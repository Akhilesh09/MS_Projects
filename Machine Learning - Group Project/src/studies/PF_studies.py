#!/usr/bin/env python3

import os, shutil, glob
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as axes3d
from matplotlib.ticker import MaxNLocator
from project import Project
from progressbar import progressbar
from utilities.searches import grid_search
from learners.perceptron_forest import PerceptronForest
import pickle as pl

def perceptron_forest_study(data, trees, fracs, num_seeds=5,
                            modes=["with", "without"], filename=None):
    """
    Generate an accuracy vs number of decision tree plot for a perceptron forest.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
            Input data for the learning algorithms. Tuple ordered by 
            data and classifications.
    trees : numpy.ndarray
        Array containing number of trees to try in perceptron forest.
    fracs : numpy.ndarray
        Array containing fraction of training data each tree should be trained
        on.
    num_seeds : int, optional
        Number of random seeds to average results over. The default is 5.
    modes: list(str), len 1-2
        List of modes to run. Options are 'with' for running with data replacement
        and 'without' for running without data replacment. The default is both
        (['with', 'without']).
    filename : str, optional
        File to save plot to. The default is None.

    Returns
    -------
    t_w : int
        Number of trees that optimizes accuracy with replacement.
    f_w : float
        Data fraction that optimizes accuracy with replacement.
    t_w0 : int
        Number of trees that optimizes accuracy without replacement.
    f_w0 : float
        Data fraction that optimizes accuracy without replacement.

    """
    
    # Define random seeds and parameters
    np.random.seed(0) # Set seed of random number generator for reproducability
    seeds = np.random.randint(0, 1E9, num_seeds)

    # Loop through number of trees in PF
    means_w, means_wo = [], []
    errs_w, errs_wo = [], []
    print("\nPerceptron Forest Study\n")
    for n in progressbar(trees):
        for frac in fracs:
            # Define hyperparams
            # w means with replacement, wo means without replacement
            hyperparams_w = {"n_estimators" : n, "max_samples" : frac,
                             "bootstrap" : True}
            hyperparams_wo = {"n_estimators" : n, "max_samples" : frac,
                              "bootstrap" : False}
    
            # Loop through seeds
            mean_w, var_w = 0.0, 0.0
            mean_wo, var_wo = 0.0, 0.0
            for seed in seeds:
                if "with" in modes:
                    # Init learner
                    hyperparams_w["random_state"] = seed
                    PF_w = Project(data, "perceptron_forest", hyperparams_w)
                    # Evaluate model accuracy
                    d_w = PF_w.evaluate_model_accuracy()
                    
                    # Tally mean and std to averaged quantities over seed
                    mean_w += d_w[0][1]
                    var_w += d_w[0][2]**2
                
                if "without" in modes:
                    hyperparams_wo["random_state"] = seed
                    PF_wo = Project(data, "perceptron_forest", hyperparams_wo)
                    d_wo = PF_wo.evaluate_model_accuracy()
        
                    mean_wo += d_wo[0][1]
                    var_wo += d_wo[0][2]**2
    
            # Add to list
            if "with" in modes:
                means_w.append(mean_w/num_seeds)
                errs_w.append(d_w[0][3]*np.sqrt(var_w)/num_seeds)
            if "without" in modes:
                means_wo.append(mean_wo/num_seeds)
                errs_wo.append(d_wo[0][3]*np.sqrt(var_wo)/num_seeds)
    
    if "with" in modes:
        # Convert everything to np arrays
        means_w = np.array(means_w)
        means_wo = np.array(means_wo)
        
        # Make sure error bars are in (0,1)
        acc_w_upper = means_w + errs_w
        acc_w_lower = means_w - errs_w
        acc_w_upper[acc_w_upper > 1.0] = 1.0
        acc_w_lower[acc_w_lower < 0.0] = 0.0
    
    if "without" in modes:
        errs_w = np.array(errs_w)
        errs_wo = np.array(errs_wo)
    
        acc_wo_upper = means_wo + errs_wo
        acc_wo_lower = means_wo - errs_wo
        acc_wo_upper[acc_wo_upper > 1.0] = 1.0
        acc_wo_lower[acc_wo_lower < 0.0] = 0.0
    
    # Make mesh of combination of fractions and trees
    f, t = np.meshgrid(fracs, trees)
    f = f.flatten(); t = t.flatten()
    
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')
    ax.set_xlabel("Number of Decision Trees")
    ax.set_ylabel("Data Fraction / Tree")
    ax.set_zlabel("Average Mean Accuracy")
    
    if "with" in modes:
        ax.scatter(t, f, means_w, marker="o", s=2, color="blue", label="With Replacement")
        # Add error bars
        for i in range(t.size):
            x = t[i]; y = f[i]; low_w = acc_w_lower[i]; high_w = acc_w_upper[i]
            ax.plot([x,x], [y,y], [low_w, high_w], marker="_", color="blue")
    if "without" in modes:
        ax.scatter(t, f, means_wo, marker="o", s=2, color="green", label="Without Replacement")
        # Add error bars
        for i in range(t.size):
            x = t[i]; y = f[i]; low_wo = acc_wo_lower[i]; high_wo = acc_wo_upper[i]
            ax.plot([x,x], [y,y], [low_wo, high_wo], marker="_", color="green")
    
    ax.legend(loc="best", markerscale=2)
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    
    if filename is not None:
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
            
        fig.savefig(filename, bbox_inches="tight")
        filebase, extention = filename.rsplit(".", 1)
        pl.dump(fig, open(filebase + ".fig", "wb"))
        
    else:
        plt.show()
    
    best_params = []
    
    # Determine optimal hyper-parameters for best accuracy
    if "with" in modes:
        best_acc_w = acc_w_lower.max()
        best_match_w = acc_w_upper[acc_w_upper - best_acc_w > 0].min()
        t_w, f_w = t[acc_w_upper == best_match_w].min(), f[acc_w_upper == best_match_w].min()
        best_params.append(t_w); best_params.append(f_w)
    
    if "without" in modes:
        best_acc_wo = acc_wo_lower.max()
        best_match_wo = acc_wo_upper[acc_wo_upper - best_acc_wo > 0].min()
        t_wo, f_wo = t[acc_wo_upper == best_match_wo].min(), f[acc_wo_upper == best_match_wo].min()
        best_params.append(t_wo); best_params.append(f_wo)
    
    return best_params

def load_perceptron_forest_study(filename, elevation=20, azimuthal=-150, filename_new=None):
    """
    Function to open pickled 3D figure and view from another angle.

    Parameters
    ----------
    filename : str
        File containing pickled object.
    elevation : float, optional
        Elevation of the 3D view. The default is 20
    azimuthal : float, optional
        Azimuthal angle of the 3D view. The default is -150 degrees
    filename_new : str, optional
        File to save figure to, if any. The default is None.
    """
    
    fig = pl.load(open(filename, "rb"))
    fig.canvas.draw()
    ax = fig.axes[0]
    
    # Set view
    ax.elev = elevation; ax.azim = azimuthal
    
    plt.tight_layout()
    
    if filename_new is not None:
        # Make sure directory exists
        file_dir = filename_new[:filename_new.rfind("/")]
        if "/" in filename_new and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
            
        fig.savefig(filename_new, bbox_inches="tight")
        
    else:
        plt.show()

def PF_grid_search(data, data_opts, k=30, n_repeats=10, filename=None, seed=3):
    """
    Perform a grid search over hyper-parameters to find the most accurate
    perceptron forest.

    Parameters
    ----------
    data : tuple of pandas.DataFrame
        Tuple containing the training data in (X, y) format.
    data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
    k : int
        Number of cross-validation folds to use in grid search.
    n_repeats : int
        Number of times to repeat k-fold CV.
    filename : str, optional
        File to save resulting plot to. The default is None.
    seed : int
        Random seed for data splitting.

    Returns
    -------
    best : dict
        Dictionary of the hyper-parameters giving the highest accuracy.

    """
    
    # Parameters to search over
    params={"n_estimators" : np.arange(1,6),
            "max_samples" : np.arange(0.05, 0.55, 0.05),}
    
    estimator = PerceptronForest(data_opts=data_opts)
    
    return grid_search(data, k, n_repeats, filename, seed, params, estimator)
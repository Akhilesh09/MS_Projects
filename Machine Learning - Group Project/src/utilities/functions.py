#!/usr/bin/env python3

import os
import pandas as pd
import numpy as np
from project import Project
from learners.perceptron import Perceptron
from learners.knn import KNN
from learners.perceptron_forest import PerceptronForest
from learners.neural_network import NeuralNetwork
from learners.ensemble_network import EnsembleNetwork
from sklearn.model_selection import StratifiedKFold, KFold, RepeatedStratifiedKFold, RepeatedKFold
import warnings

#plt.rc("text", usetex=True)

def get_classifications(programs_path="../parsed_data/programsnames_941.txt"):
    """
    Function to read data to use ML on.

    Parameters
    ----------
    data_dir : str
        Path to directory containing data files.

    Returns
    -------
    pandas.DataFrame, numpy.ndarray
        Data inputs and corresponding classifications.

    """
    # Load program names
    programs = np.loadtxt(programs_path, dtype=str)
    
    classif = []
    for program in programs:
        if 'calibration' in program:
            classif.append(1)
        elif 'Flush' in program:
            classif.append(1)
        elif 'Meltdown' in program:
            classif.append(1)
        elif 'Prime' in program:
            classif.append(1)
        elif 'Spectr' in program:
            classif.append(1)
        elif 'breaking' in program:
            classif.append(1)
        else:
            classif.append(-1)
    return pd.DataFrame(classif, index=programs)        

def generate_cv(y, k, n_repeats, seed):
    """
    Generate a cross-validation object

    Parameters
    ----------
    y : numpy.ndarray
        Array of training data classifications.
    k : int
        Number of cross-validation folds.
    n_repeats : int
        Number of time to repeat K-fold CV.
    seed : int
        Random seed for data splitting.

    Returns
    -------
    cv : (Repeated)StratifiedKFold or (Repeated)KFold
        Object for performing cross-validation.

    """
    
    # Determine whether to use k-fold or stratified k-fold
    num_pos = np.sum(y==1)
    num_neg = y.size - num_pos
    
    if k > y.size:
        warnings.warn("Requested {0} folds too many for data-set with {1} "
                      "samples. Using {1} folds instead.".format(k, y.size))
        k = y.size
    
    if n_repeats == 1:
        cv_class = StratifiedKFold if k < min(num_pos, num_neg) else KFold
        cv = cv_class(n_splits=k, shuffle=True, random_state=seed)
    else:
        cv_class = RepeatedStratifiedKFold if k < min(num_pos, num_neg) else RepeatedKFold
        cv = cv_class(n_splits=k, n_repeats=n_repeats, random_state=seed)
        
    return cv
    
def process_gridsearch_output(search_res):
    """
    Process output from a GridSearchCV object.

    Parameters
    ----------
    search_res : sklearn.model_selection.GridSearchCV
        An object whose fit method has already been called.

    Returns
    -------
    dict
        Parameter-value pairs that give the highest accuracy.
    combinations : list
        List of possible parameter combinations.
    accuracy_scores : dict
       (accuracy, standard deviation) pairs for each parameter combination.

    """
    
    combinations = search_res.cv_results_['params']
    combination_dict = {}
    accuracy_scores = {}
    for combination in combinations:
        combination_dict[str(combination)] = {}
        for key in search_res.cv_results_.keys():
            if not key.startswith('param'):
                combination_dict[str(combination)][key] = list(search_res.cv_results_[key])[combinations.index(combination)]     
        accuracy_scores[str(combination)] = (combination_dict[str(combination)]['mean_test_score'],
                                             combination_dict[str(combination)]['std_test_score'])
    
    return search_res.best_params_, combinations, accuracy_scores
    
            
def grid_search_cv_plot(params,combinations,accuracies,filename=None):
    """
    Shows 2 best parameters vs accuracy plot for the model.
    
    Parameters
    ----------
    params: list
        List of best parameter-value pairs.
    
    combinations: list
        List of possible parameter combinations.
    
    accuracies: list
        List of possible parameter combination-accuracy pairs.
    
    """
    
    # Get names of varied parameters
    plot_axes=[x for x in params.keys() if "random_state" not in x]
    ndim = len(plot_axes)
    assert ndim <= 3
    
    # Get list of points
    points = {name: [] for name in plot_axes}
    points["accuracy"] = []
    points["err"] = []
    for comb in combinations:
        points["accuracy"].append(accuracies[str(comb)][0])
        points["err"].append(1.96*accuracies[str(comb)][1])
        for name, value in comb.items():
            if "random_state" not in name:
                try:
                    if type(value) != int:
                        value = np.format_float_scientific(value, precision=2, trim='-')
                except TypeError:
                    pass
                points[name].append(str(value))
    
    # Get all the unique values for each component of the points
    values_unique = {name : np.unique(value) for name, value in points.items() if name != "accuracy" and name != "err"}
    # Sort x, y, z axis by number of unique values for aesthetic purposes
    axes_names, axes_sizes = zip(*values_unique.items())
    axes_sizes = [x.size for x in axes_sizes]
    ind_sort = np.argsort(axes_sizes)[::-1]
    plot_axes = np.array(plot_axes)[ind_sort]
    
    # best accuracy
    min_acc, max_acc = min(points['accuracy']), max(points['accuracy'])
    
    # Create mapping to map parameter values (strings) to numerical values
    mapping = {}
    for name, array in values_unique.items():
        try:
            ind = np.argsort(np.asarray(array, dtype=float))
            array = array[ind]
        except ValueError:
            pass
        
        mapping[name] = {array[i] : i for i in range(len(array))}
    
    fig = plt.figure()
    ax = fig.add_subplot(111) if ndim == 1 else plt.axes(projection='3d')
    ax.set_title("Accuracy Range: [{:.4f}, {:.4f}]".format(min_acc, max_acc))
    
    if ndim < 3:
        
        accs = np.array(points['accuracy'])
        errs = np.array(points["err"])
        
        err_upper = accs + errs
        err_upper[err_upper > 1.0] = 1.0
        err_upper -= accs
        err_lower = accs - errs
        err_lower[err_lower < 0.0] = 0.0
        err_lower = accs - err_lower
    
    if ndim == 1:
    
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel("Accuracy")
        
        xs = points[plot_axes[0]]
        
        try:
            xs = np.asarray(xs, dtype=float)
            ax.errorbar(xs, accs, yerr=[err_lower, err_upper], capsize=2) 
              
        except:
            xs = [mapping[plot_axes[0]][x] for x in xs]
            ax.errorbar(xs, accs, yerr=[err_lower, err_upper], capsize=2)
            
            ax.set_xticks(list(mapping[plot_axes[0]].values()))
            ax.set_xticklabels(list(mapping[plot_axes[0]].keys()))
            
        
    
    elif ndim == 2:
        
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel(plot_axes[1])
        ax.set_zlabel("Accuracy")
        
        xs = points[plot_axes[0]]
        ys = points[plot_axes[1]]
        
        set_xticks = False
        try:
            xs = np.asarray(xs, dtype=float)
              
        except:
            set_xticks = True
            xs = [mapping[plot_axes[0]][x] for x in xs]
            
        set_yticks = False
        try:
            ys = np.asarray(ys, dtype=float) 
              
        except:
            set_yticks = True
            ys = [mapping[plot_axes[1]][y] for y in ys]
        
        if set_xticks:
            ax.set_xticks(list(mapping[plot_axes[0]].values()))
            ax.set_xticklabels(list(mapping[plot_axes[0]].keys()))
        if set_yticks:
            ax.set_yticks(list(mapping[plot_axes[1]].values()))
            ax.set_yticklabels(list(mapping[plot_axes[1]].keys()))
            
        ax.scatter(xs, ys, accs, marker='o', s=2, color='blue')
        # Add error bars
        for i in range(accs.size):
            acc = accs[i]
            x = xs[i]; y = ys[i]; low_w = acc - err_lower[i]; high_w = acc + err_upper[i]
            ax.plot([x,x], [y,y], [low_w, high_w], marker="_", color="blue")
    
    elif ndim == 3:
        
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel(plot_axes[1])
        ax.set_zlabel(plot_axes[2])
        
        xs = points[plot_axes[0]]
        ys = points[plot_axes[1]]
        zs = points[plot_axes[2]]
        
        # Set up point sized based on accuracy
        min_size, max_size = 10, 100 # point size bounds
        # 100%/20 = 5% confidence interval will appear as point twice the size of
        # data point
        err_scale = 20
        # Size each point based on accuracy
        points['size'] = np.array(points['accuracy'])
        points['err_size'] = 1.0 + err_scale * np.array(points['err']) / points['size']
        # Set sizes between min_size, max_size
        points['size'] *= (max_size - min_size) / (max(points['size']) - min(points['size']))
        points['size'] += min_size - min(points['size'])
        points['err_size'] *= points['size']
        
        ss = points['size']
        es = points['err_size']
    
        for x, y, z, s, e in zip(xs, ys, zs, ss, es):
            col = 'blue' if np.isclose(s, max_size) else 'black'
            x_map = mapping[plot_axes[0]][x]
            y_map = mapping[plot_axes[1]][y]  
            z_map = mapping[plot_axes[2]][z]  
            
            # Plot error
            ax.scatter(x_map, y_map, z_map, s=e, c='red')
            # Plot accuracy
            ax.scatter(x_map, y_map, z_map, s=s,c=col)
        
        ax.set_xticks(list(mapping[plot_axes[0]].values()))
        ax.set_yticks(list(mapping[plot_axes[1]].values()))
        ax.set_zticks(list(mapping[plot_axes[2]].values()))
        
        ax.set_xticklabels(list(mapping[plot_axes[0]].keys()))
        ax.set_yticklabels(list(mapping[plot_axes[1]].keys()))
        ax.set_zticklabels(list(mapping[plot_axes[2]].keys()))
    
    plt.tight_layout()
    
    if filename is not None:
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
        fig.savefig(filename, bbox_inches="tight")
    else:
        plt.show()

def grid_search(data, k=30, n_repeats=10, filename=None, seed=3, params=None, estimator=None):
    """
    Perform a grid search over hyper-parameters to find the most accurate model.

    Parameters
    ----------
    data : tuple of pandas.DataFrame
        Tuple containing the training data in (X, y) format.
    k : int
        Number of cross-validation folds to use in grid search.
    n_repeats : int
        Number of times to repeat k-fold CV.
    filename : str, optional
        File to save resulting plot to. The default is None.
    seed : int
        Random seed for data splitting.
    params : dict
        Dictionary of parameters defining grid to search.
    estimator : sklearn estimator
        Learning model to evaluate.

    Returns
    -------
    best : dict
        Dictionary of the hyper-parameters giving the highest accuracy.

    """
    
    if params is None or estimator is None:
        raise ValueError("Must pass in arguments for 'params' and 'estimator' arguments.")
    
    X, y = data
    
    X = np.asarray(X)
    y = np.asarray(y).ravel()
    
    # Generate cross-validation object
    cv = generate_cv(y, k, n_repeats, seed)
    search_res=GridSearchCV(estimator=estimator, param_grid=params, cv=cv)
    search_res.fit(X,y)
    
    best, combs, accs = process_gridsearch_output(search_res)
    grid_search_cv_plot(best, combs, accs, filename)
    
    return best
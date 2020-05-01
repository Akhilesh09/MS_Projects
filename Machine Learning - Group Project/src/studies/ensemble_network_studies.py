#!/usr/bin/env python3
from utilities.searches import grid_search
from learners.ensemble_network import EnsembleNetwork
from project import Project


import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d.axes3d as axes3d
from matplotlib.ticker import MaxNLocator
import pickle as pl
from itertools import combinations_with_replacement
from progressbar import progressbar


def ensemble_network_grid_search(data, data_opts, k=30, n_repeats=10, filename=None, seed=3):
    """
    Perform a grid search over hyper-parameters to find the most accurate
    neural network.
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

    # Set of architectures
    layer_sizes = range(2,6)
    layers = 2
    tests = list(combinations_with_replacement(layer_sizes, layers))

    n_estimators = [4,6,8,12,16,20]

    # Parameters to search over
    params={"hidden_layer_sizes" : tests,
            "n_estimators" : n_estimators
    }

    estimator = EnsembleNetwork(data_opts=data_opts)
    
    return grid_search(data, k, n_repeats, filename, seed, params, estimator)

def ensemble_network_architecture_study(data, opts={}, num_seeds=1, layers=[1, 2, 3], 
                                      neurons_per_layer=[2, 4, 6, 8], kcv=3, 
                                      filename=None):
    """
    Generate a plot of neural net accuracy and as a function of number of layers
    and neurons/layer.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the learning algorithms. Tuple ordered by data and
        classifications.
    opts : dict, optional
        Options for the normalizations within the Project class' evaluate
        mode laccuracy. Default is empty dict.
    num_seeds : int, optional
        Number of random seeds to average results over (number of bootstrap
        samples). The default is 1.
    max_layers : int, optional
        The maximum number of layers to check. The default is 1.
    max_neuron_per_layer : int, optional
        The maximum number of neurons/layer to use. The default is 10.
    kcv : int, optional
        The number of folds to use in k-fold cross validation. 
        The default is 3.
    filename : "str", optional
        File to save plot to, if any. The default is None.
    """

    print(filename)

    # Generate random number seed
    np.random.seed(0) # Fix seed for generating seeds
    # Generate seeds
    seeds = np.random.randint(low=0, high=1E9, size=num_seeds)

    # Loop through random seeds
    print("\nEnsemble Network Architecture Study\n")
    # Looping through number of estimators
    for e_ind in range(4, 11, 2):
        print("\n--------------------------------------------------------------")
        print("Running with {} estimators".format(e_ind))
        print("--------------------------------------------------------------\n")

        # Initialize containers
        av_accs, var = np.zeros((len(layers), len(neurons_per_layer))), \
                       np.zeros((len(layers), len(neurons_per_layer)))

        # Looping through seeds
        for seed in progressbar(seeds):

            # Looping through possible layer numbers
            for l_ind, layer in enumerate(layers):

                # Looping through possible neureons/layer
                for n_ind, neurons in enumerate(neurons_per_layer):

                    # Creating actual layers structure
                    layer_structure = [neurons for _ in range(layer)]
                    layer_structure = tuple(layer_structure)
            
                    # Creating input parameters for neural net
                    params = {"hidden_layer_sizes" : layer_structure, "activation" : "relu",
                        "solver" : "adam", "learning_rate" : "adaptive",
                        "momentum" : 0.9, "batch_size" : "auto", "random_state_tree" : seed,
                        "random_state_net" : seed, "random_state_bagger" : seed,
                        "n_estimators" : e_ind}

                    # Crating neural net
                    en = Project(data, "ensemble_network", params, data_opts=opts, k=kcv)

                    # Evaluation k-fold accuracy
                    outp = en.evaluate_model_accuracy()
        
                    # Filling the containers
                    av_accs[l_ind,n_ind] += outp[0][1] # avg acc
                    var[l_ind,n_ind] += outp[0][2]**2 # std**2

        #  Averaging the dataand preparing the correct objects for the 3D plot
        av_accs = av_accs.flatten(order='F')
        var = var.flatten(order='F')

        av_accs /= num_seeds
        var /= num_seeds**2
        std = np.sqrt(var)
        # Score is always the same (z score) if number of folds = 1
        errs = outp[0][3] * std

        # Additional data for the 3D plot
        acc_upper =  av_accs + errs
        acc_lower = av_accs - errs

        # Make mesh of combination of fractions and trees
        f, t = np.meshgrid(layers, neurons_per_layer)
        f = f.flatten(); t = t.flatten()

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d')
        ax.set_xlabel("Number of layers")
        ax.set_ylabel("Neurons/layer")
        ax.set_zlabel("Average Mean Accuracy")

        print("--------------------------------------------------------------")
        print("Best model: (Layer/neurons_p_layer/Avg. acc.)")
        maxidx = np.argmax(av_accs)
        print(f[maxidx], t[maxidx], av_accs[maxidx])
        # Creating best layers structure
        layer_structure = [t[maxidx] for _ in range(f[maxidx])]
        layer_structure = tuple(layer_structure)

        # Creating input parameters for neural net
        params = {"hidden_layer_sizes" : layer_structure, "activation" : "relu",
            "solver" : "adam", "learning_rate" : "adaptive",
            "momentum" : 0.9, "batch_size" : "auto", "random_state_tree" : seed,
            "random_state_net" : seed, "random_state_bagger" : seed, 
            "n_estimators" : e_ind}

        # Crating ensemble net
        en = Project(data, "ensemble_network", params, data_opts=opts, 
                                k=kcv, split_seed=3, verbosity=1)

        # Evaluation k-fold accuracy
        en.evaluate_model_accuracy()
        print("--------------------------------------------------------------")

        ax.scatter(f, t, av_accs, marker="o", s=2, color="blue")
        # Add error bars
        for i in range(t.size):
            x = f[i]; y = t[i]; low = acc_lower[i]; high = acc_upper[i]
            ax.plot([x,x], [y,y], [low, high], marker="_", color="blue")

        ax.xaxis.set_major_locator(MaxNLocator(integer=True))
        ax.yaxis.set_major_locator(MaxNLocator(integer=True))

        if filename is not None:
            # Make sure directory exists
            fname = filename.replace(".pdf","_N"+str(e_ind)+".pdf")
            fname = os.path.basename(fname)
            file_dir = os.path.dirname(fname)


            if file_dir is not '' and not os.path.isdir(file_dir):
                os.makedirs(file_dir)

            fig.savefig(fname, bbox_inches="tight")
            filebase, extention = fname.rsplit(".", 1)
            pl.dump(fig, open(filebase + ".fig", "wb"))
        
        else:
            plt.show()

def analyze_best_model(filename, data, seed, opts={}, kcv=3):
    """
    Function to open pickled 3D figure and view from another angle.

    Parameters
    ----------
    filename : str
        File containing pickled object.
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the learning algorithms. Tuple ordered by data and
        classifications.
    opts : dict, optional
        Options for input normalization. Default is empty dict.
    elevation : float, optional
        Elevation of the 3D view. The default is 20
    azimuthal : float, optional
        Azimuthal angle of the 3D view. The default is -150 degrees
    filename_new : str, optional
        File to save figure to, if any. The default is None.
    """

    best_model = None
    best_params = None
    best_acc = 0. 
    best_time = 100. # init time to beat
    n_est = [4, 6, 8, 10]
    for e_ind in n_est:
        # Get data
        fname = filename.replace(".fig", "_N{}.fig".format(e_ind))
        fig = pl.load(open(fname, "rb"))
        ax = fig.get_axes()[0]
        collection = ax.collections[0]

        # Get best model index
        x,y,z = collection._offsets3d
        maxidx = np.argmax(z)

        # Generate best model params
        layer_structure = [int(round(y[maxidx])) for _ in range(int(round(x[maxidx])))]
        layer_structure = tuple(layer_structure)
        params = {"hidden_layer_sizes" : layer_structure, "n_estimators" : e_ind}

        # Create evaluator
        # msg = str(params)
        # msg = "\n"+"-"*len(msg)+"\n"+msg+"\n"+"-"*len(msg)
        # print(msg)

        en = Project(data, "ensemble_network", params, opts, split_seed=seed, verbosity=0)
        outp = en.evaluate_model_accuracy()
        acc = outp[0][1]

        if acc > best_acc:
            best_params = {"layers" : x[maxidx], "neurons_per_layer" : y[maxidx],
                            "n_estimators" : e_ind}
            best_model = en
            best_model.verbosity = 1
        
    return best_model, best_params

        





        
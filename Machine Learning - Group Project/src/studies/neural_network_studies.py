#!/usr/bin/env python3

import os
import numpy as np
import matplotlib.pyplot as plt
from cycler import cycler
import mpl_toolkits.mplot3d.axes3d as axes3d
from matplotlib.ticker import MaxNLocator
import pickle as pl
from project import Project
from progressbar import progressbar
from utilities.searches import grid_search
from learners.neural_network import NeuralNetwork

def neural_network_convergence_study(data, opts={}, num_seeds=1, batch_sizes=[1], 
                                     activations=["tanh"], filename=None):

    """
    Generate a plot of neural net epoch number and training time as function
    of the batch size.

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
    batch_sizes : list(int), optional
        A list describing the batch sizes to check. 
        1 = stochastic gradient descend.
        number of samples = batch gradient descend
        in between = mini-batch gradient descend.
        The default is one batch with a size of 1 (stochastic grad. desc.).
    activations : list(str), optional
        The list of activation functions to test. The default is testing 
        tanh only.
    filename : "str", optional
        File to save plot to, if any. The default is None.
    """

    # Generate random number seed
    np.random.seed(0) # Fix seed for generating seeds
    # Generate seeds
    seeds = np.random.randint(low=0, high=1E9, size=num_seeds)

    # Initialize containers
    conv_iters = np.zeros((len(batch_sizes), len(activations)))
    train_time = np.zeros((len(batch_sizes), len(activations)))

    # Getting the maximum number of samples
    no_samples = len(data[0].index)

    # Loop through random seeds
    print("\nNeural Network Convergence Study\n")
    for seed in progressbar(seeds):

        # looping over the activations
        for a_ind in range(len(activations)):

            # looping over the different batch sizes
            for b_ind in range(len(batch_sizes)):
        
                # Parameters for the neural network instance
                params = {"hidden_layer_sizes" : (7, 7, 7, 7), 
                          "activation" : activations[a_ind],
                          "solver" : "adam", 
                          "learning_rate" : "adaptive",
                          "momentum" : 0.9, 
                          "batch_size" : batch_sizes[b_ind], 
                          "random_state" : seed}

                neural_network = Project(data, "neural_network", params, data_opts=opts, k=1)

                # Evaluating the neural net on a single batch consisting of 
                # 66% of the total data
                outp = neural_network.evaluate_model_accuracy()
    
                # Filling the containers with iteration number and train time
                conv_iters[b_ind, a_ind] += neural_network.learner.learner_.n_iter_
                train_time[b_ind, a_ind] += outp[1][0]

    # Averaging these numbers over the random seeds
    conv_iters /= num_seeds
    train_time /= num_seeds

    # Plotting the results
    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.set_xlabel("Samples per batch")
    ax.set_ylabel("Number of epochs")
    ax.set_prop_cycle(cycler('linestyle', ['-', '--', ':', '-.']) +
                   cycler('marker', ['o', '^', 'x', 'v']))

    # Add the data to both axes
    for i in range(len(activations)):
        ax.plot(batch_sizes, conv_iters[:,i], label=str(activations[i]), color="k")

    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.legend(loc="best", markerscale=2)

    if filename is not None:

        # Make sure directory exists
        fname = os.path.basename(filename)
        file_dir = os.path.dirname(filename)
        if file_dir is not '' and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
            
        fig.savefig(filename, bbox_inches="tight")

    else:
        plt.show()

    # Plotting the results
    fig = plt.figure()
    ax = fig.add_subplot(111)

    ax.set_ylabel('Training time (s)')
    ax.set_xlabel("Samples per batch")

    ax.set_prop_cycle(cycler('linestyle', ['-', '--', ':', '-.']) +
                   cycler('marker', ['o', '^', 'x', 'v']))

    # Add the data to both axes
    for i in range(len(activations)):
        ax.plot(batch_sizes, train_time[:,i], label=str(activations[i]), color="b")

    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    ax.legend(loc="best", markerscale=2)

    if filename is not None:

        # Make sure directory exists
        fname = os.path.basename(filename)
        file_dir = os.path.dirname(filename)
        if file_dir is not '' and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
            
        fig.savefig(filename.replace(".pdf","_time.pdf"), bbox_inches="tight")

    else:
        plt.show()

def neural_network_architecture_study(data, opts={}, num_seeds=1, max_layers=1, 
                                      max_neuron_per_layer=10, kcv=3, 
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

    # Generate random number seed
    np.random.seed(0) # Fix seed for generating seeds
    # Generate seeds
    seeds = np.random.randint(low=0, high=1E9, size=num_seeds)

    # Initialize containers
    av_accs, var = np.zeros((max_layers, max_neuron_per_layer)), \
                   np.zeros((max_layers, max_neuron_per_layer))

    # Loop through random seeds
    print("\nNeural Network Architecture Study\n")
    for seed in progressbar(seeds):

        # Looping through possible layer numbers
        for l_ind in range(max_layers):

            # Looping through possible neureons/layer
            for n_ind in range(max_neuron_per_layer):

                # Creating actual layers structure
                layer_structure = [n_ind+1 for _ in range(l_ind+1)]
                layer_structure = tuple(layer_structure)
        
                # Creating input parameters for neural net
                params = {"hidden_layer_sizes" : layer_structure, "activation" : "relu",
                      "solver" : "adam", "learning_rate" : "adaptive",
                      "momentum" : 0.9, "batch_size" : "auto", "random_state" : seed}

                # Crating neural net
                neural_network = Project(data, "neural_network", params, data_opts=opts, k=kcv)

                # Evaluation k-fold accuracy
                outp = neural_network.evaluate_model_accuracy()
    
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
    f, t = np.meshgrid(range(1, max_layers+1), 
                       range(1, max_neuron_per_layer+1))
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
              "momentum" : 0.9, "batch_size" : "auto", "random_state" : seed}

    # Crating neural net
    neural_network = Project(data, "neural_network", params, data_opts=opts,
                             k=kcv, verbosity=1, seed=3)

    # Evaluation k-fold accuracy
    neural_network.evaluate_model_accuracy()
    print("--------------------------------------------------------------")

    ax.scatter(f, t, av_accs, marker="o", s=2, color="blue")
    # Add error bars
    for i in range(t.size):
        x = f[i]; y = t[i]; low = acc_lower[i]; high = acc_upper[i]
        ax.plot([x,x], [y,y], [low, high], marker="_", color="blue")

    ax.xaxis.set_major_locator(MaxNLocator(integer=True))

    if filename is not None:
        # Make sure directory exists
        fname = os.path.basename(filename)
        file_dir = os.path.dirname(filename)

        if file_dir is not '' and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
            
        fig.savefig(filename, bbox_inches="tight")
        filebase, extention = filename.rsplit(".", 1)
        pl.dump(fig, open(filebase + ".fig", "wb"))
        
    else:
        plt.show()

def modify_neural_network_figure(filename, elevation=20, azimuthal=240, filename_new=None):
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
    ax = fig.get_axes()[0]

    for collection in ax.collections:
        print("--------------------------------------------------------------")
        print("Best model: (Layer/neurons_p_layer/Avg. acc.)")
        x,y,z = collection._offsets3d
        maxidx = np.argmax(z)
        print(x[maxidx], y[maxidx], z[maxidx])
        print("--------------------------------------------------------------")
    
    # Set view
    ax.elev = elevation; ax.azim = azimuthal
    
    plt.tight_layout()
    
    if filename_new is not None:
        # Make sure directory exists
        fname = os.path.basename(filename)
        file_dir = os.path.dirname(filename)

        if file_dir is not '' and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
           
        fig.savefig(filename_new, bbox_inches="tight")
        
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
    
    fig = pl.load(open(filename, "rb"))
    ax = fig.get_axes()[0]

    collection = ax.collections[0]

    print("--------------------------------------------------------------")
    print("Best model: (Layer/neurons_p_layer/Avg. acc.)")

    x,y,z = collection._offsets3d
    maxidx = np.argmax(z)
    print(x[maxidx], y[maxidx], z[maxidx])

    # Creating best layers structure
    layer_structure = [int(round(y[maxidx])) for _ in range(int(round(x[maxidx])))]
    layer_structure = tuple(layer_structure)

    # Creating input parameters for neural net
    params = {"hidden_layer_sizes" : layer_structure, "activation" : "relu",
          "solver" : "adam", "learning_rate" : "adaptive",
          "momentum" : 0.9, "batch_size" : "auto", "random_state" : seed}

    # Crating neural net
    neural_network = Project(data, "neural_network", params, 
                             data_opts=opts, verbosity=1, k=kcv)

    # Evaluation k-fold accuracy
    neural_network.evaluate_model_accuracy()

    print("--------------------------------------------------------------")

def neural_net_grid_search(data, data_opts, k=30, n_repeats=10, filename=None, seed=3):
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
    
    # Parameters to search over
    params={"hidden_layer_sizes" : [(10,)],
            "momentum" : np.linspace(0.0, 1, 5),
            "alpha" : 10.0**-np.arange(4,5)}
    
    estimator = NeuralNetwork(data_opts=data_opts)
    
    return grid_search(data, k, n_repeats, filename, seed, params, estimator)

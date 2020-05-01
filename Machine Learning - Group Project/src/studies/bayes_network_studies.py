#!/usr/bin/env python3
from utilities.data_parser import DataParser
from utilities.functions import get_classifications
from utilities.searches import grid_search
from utilities.correlation_filter import correlation_filter
from project import Project

import numpy as np
from learners.bayes_network import BayesNetwork

def bayes_network_grid_search(data, data_opts, k=30, n_repeats=10, filename=None, seed=3):
    """
    Perform a grid search over hyper-parameters to find the most accurate
    Bayesian network.
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
    params={"hidden_layer_sizes" : [(10,), (15,), (20,)],
            "momentum" : np.linspace(0.0, 1, 4),
            "n_estimators" : np.arange(10,21,3)}
    
    estimator = BayesNetwork(data_opts=data_opts)
    
    return grid_search(data, k, n_repeats, filename, seed, params, estimator)

def get_bayes_network_studies(opts,data_path, programs_path,corr_threshold):
    """
    Perform a study of different normalization options on the ensemble network.
    """
	
    case = "941_486"
    outbase = os.path.join("..", "output")

    # Prepare data
    parser = DataParser()
    X = parser.parse_datafile(data_path, programs_path)
    y = get_classifications(programs_path)
    data = (X, y)
    # Filter data
    data = correlation_filter(data, threshold=corr_threshold)

    ### Run learning models
     
    msg = msg = ">"*10 + " "
    msg += "Learner evaluation phase..."
    print(msg)

    # Parameters
    split_seed = 3 # Random seed 
    verbosity = 1

    outfile = None
    best_params = bayes_network_grid_search(data, k=3, n_repeats=1, filename=outfile, seed=split_seed)

    en = Project(data, "bayes_network", best_params, split_seed=split_seed, verbosity=verbosity)
    en.evaluate_model_accuracy(opts)

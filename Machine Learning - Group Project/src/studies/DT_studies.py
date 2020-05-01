#!/usr/bin/env python3

from learners.decision_tree import DecisionTree
from utilities.searches import grid_search

def DT_grid_search(data, data_opts, k=30, n_repeats=10, filename=None, seed=3):
    """
    Perform a grid search over hyper-parameters to find the most accurate
    decision tree.

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
    params={"criterion" : ["entropy", "gini"],
            "ccp_alpha" : [0.1*i for i in range(6)],
            "max_depth" : [i for i in range(1,5)]}
    
    estimator = DecisionTree(data_opts=data_opts)
    
    return grid_search(data, k, n_repeats, filename, seed, params, estimator)
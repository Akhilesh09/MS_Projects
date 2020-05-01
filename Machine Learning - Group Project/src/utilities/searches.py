#!/usr/bin/env python3

from utilities.functions import generate_cv
from utilities.plots import grid_search_cv_plot
import numpy as np
import pandas as pd
from sklearn.model_selection import GridSearchCV
from utilities.stats import compute_confidence_score
from project import Project

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
    
    y = np.asarray(y).ravel()
    
    # Generate cross-validation object
    cv = generate_cv(y, k, n_repeats, seed)        
    
    search_res=GridSearchCV(estimator=estimator, param_grid=params, cv=cv)
    search_res.fit(X,y)
    
    DF = pd.DataFrame(search_res.cv_results_)
    DF.fillna(value="None", inplace=True)
    
    # Adjust standard deviations using prior knowledge of binomial distribution
    
    # First get number of samples in each test fold
    n_samps = {}
    fold = 0
    for itrain, itest in cv.split(X,y):
        n_samps["split{}_test_score".format(fold)] = [len(itest)]
        fold += 1
    n_samps = pd.DataFrame(n_samps)
    
    # Compute variance of each score using binomial distribution
    variances = DF[["split{}_test_score".format(i) for i in range(fold)]]
    variances *= (1.0 - variances)
    variances = variances.div(n_samps.iloc[0], axis='columns')
    
    # Overwrite original stds
    DF['std_test_score'] = variances.sum(axis=1)
    DF['std_test_score'] = DF['std_test_score'].apply(np.sqrt)
    DF['std_test_score'] /= variances.shape[1]
    
    # Get confidence score
    scores = np.array([compute_confidence_score(acc, len(y), 0.95, fold)
                       for acc in DF['mean_test_score']])
    
    # Compute confidence intervals
    DF["confidence"] = scores * DF['std_test_score']
    
    if len(params) <= 3:
        grid_search_cv_plot(DF, filename)
    
    return search_res.best_params_
    
def transformation_search(data, model, hyperparams={}, data_opts={}, num_seeds=1):

    norm_opts = ["max", "standard", "robust"]
    domain_opts = ["global", "time"]

    # Generate random number seed
    np.random.seed(0) # Fix seed for generating seeds
    # Generate seeds
    seeds = np.random.randint(low=0, high=1E9, size=num_seeds)

    result = []
    mean_acc = np.zeros((len(norm_opts), len(domain_opts)))
    train_time = np.zeros(mean_acc.shape)
    for sd, seed in enumerate(seeds):
        count = 0
        for n, norm in enumerate(norm_opts):
            for d, domain in enumerate(domain_opts):
                params = {
                    "pca_opt" : False,
                    "norm_opt" : norm,
                    "domain_opt" : domain
                }
                proj = Project(
                    data, model, hyperparams=hyperparams, 
                    data_opts=data_opts, split_seed=3, verbosity=0
                )
                outp = proj.evaluate_model_accuracy()
                mean_acc[n,d] += outp[0][1]
                if train_time[n,d] == 0.:
                    train_time[n,d] += outp[1][0]
                elif outp[1][0] < train_time[n,d]:
                    train_time[n,d] = outp[1][0]
        count += 1
    mean_acc /= num_seeds

    # Start table
    acc, times = "", ""
    opener = "\\begin{table}[H]\n"
    opener += "\t\\begin{tabular}{|c||c|c|c|}\n"
    opener += "\t\t\\hline\n"
    for d, dom in enumerate(domain_opts):
        if d == 0:
            opener += "\t\t& Max & Standard & Robust"
            opener += "\\\\ \\hline \\hline\n"
            acc += "\t\tGlobal"
            times += "\t\tGlobal"
        elif d == 1:
            acc += "\t\tTime"
            times += "\t\Time"
        for n, norm in enumerate(norm_opts):
            acc += " & {:.4f}".format(mean_acc[n,d])
            times += " & {:.4f}".format(train_time[n,d])
        acc += "\\\\ \\hline\n"
        times += "\\\\ \\hline\n"
    closer = "\t\\end{tabular}\n"
    closer += "\\end{table}"
    print(opener + acc + closer)
    print()
    print(opener + times + closer)
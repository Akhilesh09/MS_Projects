#!/usr/bin/env python3

from scipy import stats
from scipy.stats import norm, t, chi2, binom_test
import numpy as np
import warnings

def compute_confidence_score(mean_accuracy, nsamples, confidence, nfolds):
    """
    Generates the t or z score needed to compute confidence intervals.

    Parameters
    ----------
    mean_accuracy : float
        Accuracy averaged over folds.
    nsamples : int
        Cumulative number of samples accross all test folds.
    confidence : float in (0,1)
        Desired confidence interval.
    nfolds : int
        Number of folds used for cross validation.

    Returns
    -------
    score : float or None
        t or z score, whichever is appropriate. If statistics are not good
        enough, a warning is raised and None is returned.

    """
    
    # z/t-factor for given confidence with k-folds
    # ppf = percent point function (inverse cumulative distribution function)
    # 0.5 * (1.0 + confidence) because we want 2 sided CI
    if (nsamples * mean_accuracy >= 5 and
        nsamples * (1.0 - mean_accuracy) >= 5):
        score = norm.ppf(0.5 * (1.0 + confidence))
    elif nfolds > 1:
        score = t.ppf(0.5 * (1.0 + confidence), nfolds)
    else:
         score = None
         warnings.warn("The normal approximation does not hold for this test "
                       "splitting (np = {:0.0f}, n(1-p) = {:0.0f}) and a T distribution "
                       "of degree 0 does not exist"
                       ".".format(np.floor(nsamples * mean_accuracy), np.floor(nsamples * (1.0 - mean_accuracy))))

    return score

def run_mcnemar_tests(model1,model2,X_test,y_test):
    """
    Perform McNemar's Test on the test sets from 3 fold CV to compare model1 and model2.
    Calculates the test statistic using chi-square test

    Parameters
    ----------
    model1 : Project
        Learner 1
    model2 : Project
        Learner 2
    X_test : 2d pandas.DataFrame
        Data frame of sample feature values.
     y_test : numpy.ndarray
         Array of classifications corresponding to X_test.
        
    """

    y_test = y_test.flatten()

    model1_name = " ".join([x.capitalize() for x in model1.model.split("_")])
    model2_name = " ".join([x.capitalize() for x in model2.model.split("_")])

    #get predictions for models
    model1_predictions=model1.learner.predict(X_test)
    model2_predictions=model2.learner.predict(X_test)
    
    #Values in 2x2 Contingency Table
    A = np.sum((model1_predictions == y_test) & (model2_predictions == y_test))
    B = np.sum((model1_predictions == y_test) & (model2_predictions != y_test))
    C = np.sum((model1_predictions != y_test) & (model2_predictions == y_test))
    D = np.sum((model1_predictions != y_test) & (model2_predictions != y_test))

    # Compute p-value
    if B > 50 and C > 50: # value of 50 from Raschka
        # Compute p-value from chi^2 score

        # Compute chi squared
        try:
            chi_sq=(abs(B - C) - 1.0)**2/(B+C)
        except:
            chi_sq=0.0
        
        p = chi2.sf(x=chi_sq, df=1)
    
    else:
        # Compute exact p-value from binomial test
        p = binom_test(x=[B, C], p=0.5)
    
    table = "\\begin{table}[H]\n"
    table += "\t\\centering\n"
    table += ("\t\\caption{{McNemar test results for {} and {} (p = {:.3e})}}\n"
              .format(model1_name.lower(), model2_name.lower(), p))
    table += "\t\\begin{tabular}{|c||c|c|}\n"
    table += "\t\t\\hline\n"
    table += "\t\t& {0} Correct & {0} Incorrect \\\\ \\hline \\hline\n".format(model1_name)
    table += "\t\t{} Correct & {:d} & {:d} \\\\ \\hline\n".format(model2_name, A, B)
    table += "\t\t{} Incorrect& {:d} & {:d} \\\\ \\hline\n".format(model2_name, C, D)
    table += "\t\end{tabular}\n"
    table += "\end{table}\n"

    print(table)

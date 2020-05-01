#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt

from utilities.correlation_filter import correlation_filter

def feature_feature_correlation_study(data, filename=None, tag="", verbosity=0):
    """
    Generate a plot of the number of features retained as function of the 
    correlation filter threshold.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the correlation filter. Tuple ordered by data and
        classifications.
    filename : "str", optional
        File to save plot to, if any. The default is None.
    tag : "str", optional
        Tag tha appears in the output filenames. Default is nothing.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0.
    """

    # Creating possible thresholds
    thresholds = np.linspace(0.5, 1.0, num=20)

    # Initializing containers for feature numbers
    number_of_features_la = []
    number_of_features_pearson = []

    # Getting number of features at the beginning
    n_features = len(data[0].columns)

    # Looping over the threshold values
    for threshold in thresholds:

        # Filtering data with Pearson correlation
        new_data = correlation_filter(data, filter_type="feature", corr_type="pearson", 
                                      threshold=threshold, verbosity=verbosity)

        number_of_features_pearson.append(len(new_data[0].columns))

        # Filtering data using LA correlation
        new_data = correlation_filter(data, filter_type="feature", corr_type="la", 
                                      threshold=threshold, verbosity=verbosity)

        number_of_features_la.append(len(new_data[0].columns))
    
    # Plot values
    fig, ax = plt.subplots()
    ax.set_xlabel("Correlation threshold", fontsize=14)
    ax.set_ylabel("Number of features", fontsize=14)
    ax.plot(thresholds, number_of_features_pearson, "k", label="Pearson")
    ax.plot(thresholds, number_of_features_la, ":b", label="Lin. Alg.")
    ax.legend(loc="best")
    ax.set_ylim(0, 1.1*n_features)
    fig.tight_layout()
    
    # Save or show
    if filename is not None:
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
        base, ext = filename.rsplit(".")
        fig.savefig("{}.{}".format(base, ext))
    else:
        plt.show()

def feature_label_correlation_study(data, outdir=None, tag="", verbosity=0):

    """
    Generate and print sorted lists based on the feature-label correlations.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the correlation filter. Tuple ordered by data and
        classifications.
    outdir : "str", optional
        Where we want to print the txt results. The default is None.
    tag : "str", optional
        Tag tha appears in the output filenames. Default is nothing.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0.
    """

    # Filtering data with Pearson correlation
    new_data = correlation_filter(data, outdir=outdir, case="pearson_"+tag+"_", 
                                  filter_type="label", corr_type="pearson", 
                                  threshold=1.0, verbosity=verbosity)

    # Filtering data using LA correlation
    new_data = correlation_filter(data, outdir=outdir, case="la_"+tag+"_", 
                                  filter_type="label", corr_type="la", 
                                  threshold=1.0, verbosity=verbosity)

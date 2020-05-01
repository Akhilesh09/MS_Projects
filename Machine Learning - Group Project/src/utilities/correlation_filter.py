#!/usr/bin/env python3

import os, shutil, glob, sys
import copy
import pandas as pd
import numpy as np
from numpy import linalg as la

def create_filtered_data_sets(data, data_name, outdir, filter_type="feature", corr_type="pearson", 
                              thresholds=[0.99], verbosity=0):
    """
    Creating filtered data bases.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the correlation filter. Tuple ordered by data and
        classifications.
    data_name : str
        String containing the name of the current data base.
    outdir : str, optional
        Path to directory where we want to output the new data bases. 
    filter_type : str, optional
        The filter type we want to use.
            "feature": Use feature-feature correlation.
            "label": Use feature-label correlation
        The default is "feature".
    corr_type : str, optional
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
        The default is "feature".
    thresholds : list(float), optional
        The thresholds for the reduced data bases.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0.

    """
    
    # Looping over the thresholds
    for threshold in thresholds:

        # Creating filtered data base
        new_data = correlation_filter(data, filter_type=filter_type, 
                                      corr_type=corr_type, threshold=threshold,
                                      verbosity=verbosity)

        # Printing new data base in the given format
        filename =  data_name.split(".")[0] + "_" + filter_type + \
                    "_" + corr_type + "_" + "{:.0%}".format(threshold)[:-1] + ".txt"
        filename = os.path.join(outdir, filename)
        new_data[0].T.to_csv(filename, sep=",", index=True, header=False, float_format="%.8f")


def correlation_filter(data, outdir=None, case="", filter_type="feature", corr_type="pearson", 
                       threshold=0.99, verbosity=0):
    """
    Filtering out features based on feature-feature or feature-label
    correlations.

    Parameters
    ----------
    data : (pandas.DataFrame, pandas.DataFrame)
        Input data for the correlation filter. Tuple ordered by data and
        classifications.
    outdir : str, optional
        Path to directory where we want to output the correlated groups and 
        remaining features. If None, if skips printing the results.
        Default is None.
    filter_type : str, optional
        The filter type we want to use.
            "feature": Use feature-feature correlation.
            "label": Use feature-label correlation
        The default is "feature".
    corr_type : str, optional
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
        The default is "feature".
    threshold : float, optional
        The cut-off threshold for the features in the correlation filter.
        This is used for both correlation filters as follows:
            filter_Type = "feature": Threshold refers to the correlation 
                                     coefficients between features
            filter_type = "label": Threshold is the ratio of features we 
                                   want to keep.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0. 
    """

    # Running few checks to see if the correct input is given
    if filter_type not in ["feature", "label"]:
        raise ValueError("Wrong correlation filter type. It has to "
                         "be (\"feature/label\")")
    if corr_type not in ["pearson", "la"]:
        raise ValueError("Wrong correlation type. It has to "
                         "be (\"pearson/la\")")
    
    # Preparing copy to make sure we do not modify the original data
    new_data = copy.deepcopy(data)
    old_number_of_features = len(new_data[0].columns)
    new_number_of_features = old_number_of_features

    # Filtering data out and perparing a new data base
    if filter_type == "feature":

        data_table, correlated_groups = filter_correlated_features(data[0], 
                                                                   corr_type, 
                                                                   threshold,
                                                                   verbosity)
        new_data = (data_table, new_data[1])

        if outdir != None:
            # Printing the correlated groups of features
            filename = "correlated_feature_groups_"+ case + \
                       str(old_number_of_features)+ " " + \
                        "_"+"{:.0%}".format(threshold)[:-1] + ".txt"
            outpath = os.path.join(outdir, filename)
            out_file = open(outpath, "w")
            out_text = "\n".join([" ".join(i) for i in correlated_groups])
            out_file.write(out_text)
            out_file.close()

    elif filter_type == "label":

        data_table, corr_vector = filter_label_correlated_features(data, 
                                                                   corr_type, 
                                                                   threshold,
                                                                   verbosity)

        new_data = (data_table, new_data[1])

        if outdir != None:
            # Printing the correlated the sorted features
            filename = "sorted_label_correlations_" + case + \
                       str(old_number_of_features) + \
                        "_" + "{:.0%}".format(threshold)[:-1] + ".txt"
            outpath = os.path.join(outdir, filename)
            out_file = open(outpath, "w")
            out_text = [" ".join([corr_vector[0][i], str(corr_vector[1][i])]) 
                        for i in range(len(corr_vector[0]))]
            out_text = "\n".join(out_text)
            out_file.write(out_text)
            out_file.close()

    if outdir != None:
        # Printing the remaining features into file
        filename = "corr_filtered_features_" + case + \
                   str(old_number_of_features) + \
                   "_" + "{:.0%}".format(threshold)[:-1] + ".txt"
        outpath = os.path.join(outdir, filename)
        out_file = open(outpath, "w")
        out_text = "\n".join(new_data[0].columns)
        out_file.write(out_text)
        out_file.close()

    return new_data        

def filter_correlated_features(data, corr_type, threshold, verbosity=0):

    """
    Filtering out features based on feature-feature correlations.

    Parameters
    ----------
    data : pandas.DataFrame
        Input data for the correlation filter. DataFrame containing the
        values foe each sample and feature combination.
    corr_type : str
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
    threshold : float
        The cut-off threshold for the features in the correlation filter.
        This threshold refers to the correlation coefficients in the 
        correlation matrix.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0.
    """

    # We need a copy of the data base to avoid accidentaly modifying 
    # the original
    data_table = copy.deepcopy(data)

    correlated_groups = []
    correlated = True

    if verbosity > 0:
        print("------------------------------------------------------------------")
        print("Features at the beginning of the iteration: "
                +str(len(data_table.columns)))
    
    # Grouping the features until we still find correlated pairs
    while(correlated):
      

        correlated = False
      
        # Creating correlation matrix
        corr_mx = create_correlation_matrix(data_table, corr_type)

        # Looping over the correlation matrix and selecting the correlated features
        for row_index in range(len(corr_mx)):
        
            # List containing the non correlated features (the ones we want to keep)
            filtered_features = []
            for col_index in range(len(corr_mx)):
          
                if(row_index==col_index):
                    filtered_features.append(data_table.columns[col_index])
                    continue
          
                if(abs(corr_mx[row_index][col_index]) < threshold + 2*sys.float_info.epsilon):
                    filtered_features.append(data_table.columns[col_index])
        
            # If all of the features are non-correlated, we go to the next row
            if(set(filtered_features) == set(data_table.columns)):
                continue
        
            # Else we discard the correlated features and keep only one from
            # the group
            else:
          
                tmp =  list(set(data_table.columns) - set(filtered_features))
                tmp.insert(0, data_table.columns[row_index])
                correlated_groups.append(tmp)
          
                data_table = data_table.filter(items=filtered_features, axis=1)
                correlated = True
                break
    
    if verbosity > 0:
        print("Features at the end of the iteration: "
                +str(len(data_table.columns)))
        print("------------------------------------------------------------------")
      
    return data_table, correlated_groups

def filter_label_correlated_features(data, corr_type, threshold, verbosity=0):

    """
    Filtering out features based on feature-feature correlations.

    Parameters
    ----------
    data : pandas.DataFrame
        Input data for the correlation filter. DataFrame containing the
        values foe each sample and feature combination.
    corr_type : str
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
    threshold : float
        The cut-off threshold for the features in the correlation filter.
        This threshold refers to the correlation coefficients in the 
        correlation matrix.
    verbosity : int, optional
        Controls the amount of output information on the terminal.
        Default is 0.
    """
               
    # Creating correlation vector
    corr_vector = create_label_correlation(data[0], data[1], corr_type)
    
    # Sorting the attributes
    idx = np.flip(np.argsort(np.abs(corr_vector[0])))
    
    # Keeping the most correlated features
    to_keep = int(threshold*len(corr_vector[0]))
    
    idx = idx[0:to_keep]
    
    filtered_features = list(data[0].columns[idx])

    filtered_corr_values = np.abs(corr_vector[0,idx])
    
    if verbosity > 0:
        print("Keeping the "+str(len(filtered_features))+" most correlated "
            "(to the label vector) features.")
    
    new_data = data[0].filter(items=filtered_features, axis=1)

    return new_data, [filtered_features, filtered_corr_values]

def create_correlation_matrix(feature_values, corr_type):

    """
    Creating feature-feature correlation matrix.

    Parameters
    ----------
    feature_values : pandas.DataFrame
        Input data that contains the sample values for each feature.
    corr_type : str
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
    """
    
    # Extracting values from Pandas DataFrame
    stripped_data = feature_values.values
    number_of_features = len(feature_values.columns)

    # Initializing the correlation matrix
    corr_mx = np.zeros((number_of_features, number_of_features))
  
    # Filling it depending on what metric we want to use
    if(corr_type == "la"):
        for i in range(number_of_features):
            norm_i = la.norm(stripped_data[:,i])
            for j in range(number_of_features):
                norm_j = la.norm(stripped_data[:,j])
                corr_mx[i][j] = np.dot(stripped_data[:,i], stripped_data[:,j])
                corr_mx[i][j] = corr_mx[i][j] / (norm_i * norm_j)
    elif(corr_type == "pearson"):
        corr_mx = np.corrcoef(stripped_data.T)

    return corr_mx


def create_label_correlation(feature_values, label_values, corr_type):

    """
    Creating feature-label correlation vector.

    Parameters
    ----------
    feature_values : pandas.DataFrame
        Input data that contains the sample values for each feature.
    label_values : pandas.DataFrame
        Input data that contains the labels for each sample.
    corr_type : str
        The correaltion type we want to use.
            "pearson": Pearson correaltion.
            "la": Linear algebra based correlation.
    """
    
    # Extracting values from Pandas DataFrame
    stripped_data = feature_values.values
    stripped_labels = label_values.values
    number_of_features = len(feature_values.columns)

    # Initializing the correlation vector
    corr_vector = np.zeros((1, number_of_features))
    
    # Filling it depending on what metric we want to use
    if(corr_type == "la"):
        for i in range(number_of_features):
            corr_vector[0][i] = np.dot(stripped_data[:,i], stripped_labels[:,0])
            corr_vector[0][i] = corr_vector[0][i] / (la.norm(stripped_data[:,i]) *
                                                 la.norm(stripped_labels[:,0]))
    elif(corr_type == "pearson"):
      for i in range(number_of_features):
        corr_vector[0][i] = np.corrcoef(stripped_data[:,i], 
                                        stripped_labels[:,0])[0][1]

    return corr_vector
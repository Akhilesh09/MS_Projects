# -*- coding: utf-8 -*-
"""
This file contains a class used to select the most correlated features and 
rank them, and 3 functions used to visualize the result.
"""
import os
from sklearn.feature_selection import SelectPercentile, chi2, f_classif

def get_best_fts_perceptron(N, filename):
    """
    Get most important features and corresponding weights from perceptron
    Parameters
    ----------
    N : int
        How many of the most important features to get.
    filename : str
        File holding feature names and perceptron weights.
    Returns
    -------
    features_ : list
        List of N most important feature names.
    weights_ : list
        list of the corresponding weights
    
    """
    import numpy as np
    
    features = open(filename, "r")
    features = [feature.rstrip() for feature in features.readlines()]
    features_ = []
    weights_ = []
    for feature in features[1:]:
        feature = feature.split(",")
        if feature[0] != "bias":
            features_.append(feature[0])
            weights_.append(float(feature[1]))
            if len(features_) == N:
                break
    max_wgt = max(weights_)
    weights_ =  [wgt/max_wgt for wgt in weights_]
    return np.array(features_), np.array(weights_)

def box_plot_for_classif(X, y, sorted_features, ft_num = 3, file_path = "newplot.png"):
    """
    Plot the box graph for a classifier

    Parameters
    ----------
    X : Dataframe
        DESCRIPTION. X data for features
    y : Dataframe
        DESCRIPTION. y data for targets
    sorted_features : list
        DESCRIPTION. Sorted features by the classifier in descending order
    ft_num : int, optional
        DESCRIPTION. The default is 3. The number of features you want to compare
    file_path : str, optional
        DESCRIPTION. The default is "newplot.png". The path to save the result.

    Returns
    -------
    None.

    """
    import matplotlib.pyplot as plt
    
    # get the features
    ft_set = sorted_features[0:ft_num]
    # separate the data
    benign_ = (y.values == -1).flatten()
    mali_ = (y.values == 1).flatten()
    
    lab_set =['benign', 'malicious']
    fig, axes = plt.subplots(1,ft_num, figsize=(12,ft_num))
    for i,ft in enumerate(ft_set):
        axes[i].boxplot([X[ft].values[benign_],X[ft].values[mali_]], 
                        patch_artist=True, labels=lab_set,
                        showmeans=True,
                        boxprops = {'color':'black','facecolor':'#9999ff'}, 
                        flierprops = {'marker':'o','markerfacecolor':'red','color':'black'},
                        meanprops = {'marker':'D','markerfacecolor':'indianred'},
                        medianprops = {'linestyle':'--','color':'orange'})
        base = ft[:ft.rfind(".")]
        ind = base.rfind(".")
        axes[i].set_title(ft[ind+1:], fontsize=12)
        axes[i].grid()
    
    # examine if there exists a valid direction
    file_dir = file_path[:file_path.rfind("/")]
    if "/" in file_path and not os.path.isdir(file_dir):
        os.makedirs(file_dir)
    plt.savefig(file_path)    

def score_compare_plot(scores, features, labels = [], base_score = 0, ft_number = 5, plt_file = 'newplot.png'):
    """
    Plot the multi-bar graph for different ranking method

    Parameters
    ----------
    scores : numpy.array(nxm)
        DESCRIPTION. The score set for all the methods. n methods, and m features.
    features : numpy.array(nxm)
        DESCRIPTION. The corresponding feature for every score in each method.
    labels : list (n elements), optional
        DESCRIPTION. The default is []. The names for all the methods
    base_score : 0 ~ n-1, optional
        DESCRIPTION. The default is 0. The row index of the scores by which we
        will select out the k features.
    ft_number : int, optional
        DESCRIPTION. The default is 5. The number of features that will be selected out.
    plt_file : string, optional
        DESCRIPTION. The default is 'newplot.png'. The path to save the result.

    Returns
    -------
    None.

    """
    import matplotlib.pyplot as plt
    import numpy as np
    
    # the number of methods
    num = len(scores[:,0])
    
    # the width of the bar in the graph
    bar_width = 0.2
    
    #default labels
    if len(labels) == 0:
        for i in range(num):
            labels.append('score'+str(i))
    else:
        for i in range(len(labels)):
            if labels[i] == "chi2":
                labels[i] = "$\chi^2$"
    
    loc = np.arange(ft_number)
    ranks = np.argsort(-scores[base_score])
    ft_set = features[base_score, ranks][0:ft_number]
    
    # get scores by feature indices
    sc_set = np.zeros((len(scores), ft_number))
    for i, ft in enumerate(ft_set):
        idx = np.argwhere(features == ft)
        for sig_idx in idx:
            vl = scores[sig_idx[0],sig_idx[1]]
            sc_set[sig_idx[0],i] = vl
    
    # Trucante feature names
    for i,ft in enumerate(ft_set):
        base = ft[:ft.rfind(".")]
        ind = base.rfind(".")
        ft_set[i] = ft_set[i][ind+1:]
    # Init figure 
    fig, ax = plt.subplots()
    # Formatting
    ax.set_ylabel("Score", fontsize=14)
    # Plotting
    for i in range(num):
        ax.bar(loc+i*bar_width, sc_set[i], width=bar_width, label=labels[i])
    
    plt.legend()
    plt.xticks(ticks=loc+i*bar_width/2, labels=ft_set, rotation="vertical")
    plt.tight_layout()
    
    # examine if there exists a valid direction
    file_dir = plt_file[:plt_file.rfind("/")]
    if "/" in plt_file and not os.path.isdir(file_dir):
        os.makedirs(file_dir)
        
    plt.savefig(plt_file)
    plt.show()
        


class FeatureSorter(object):
    
    def __init__(self, score_function=0, percent=5):
        """
        Initiate the parameters and functions

        Parameters
        ----------
        score_function : int(0 or 1), optional
            DESCRIPTION. The default is 0. When score_function=0, the test is 
            chi-square, 1 for F-test
        Percent : float(from 0 to 100), optional
            DESCRIPTION. The default is 5. The ratio of the number of selected
            features over the number of total features

        Returns
        -------
        None.

        """
        self._score_function = score_function
        self._percent = percent
        self.selector = SelectPercentile(self._get_score_func(), percentile=self._percent)
        
        # For all the input data
        self.X = None # input X dataframe
        self.y = None # input y dataframe
        self.p_values = None # the p values for all the features
        self.scores = None # the scores for all the features
        self.sorted_fts = None # sorted features
        
        # For selected data
        self.X_sorted = None # sorted dataframe
        self.selected_fts = None # selected features
        self.selected_scores = None # the scores for selected features
    
    def _data_clean(self):
        from sklearn.feature_selection import VarianceThreshold
        
        # Threshold is 0, which means the features containing only one value will be deleted
        VTS = VarianceThreshold() 
        VTS.fit(self.X.values)
        self.X = self.X.loc[:,VTS.get_support()]
    
    
    def _get_score_func(self):
        func_list = [chi2, f_classif]
        return func_list[self._score_function]
                                         
                                         
    def fit(self, X, y):
        """
        Training the object by input data

        Parameters
        ----------
        X : DataFrame
            DESCRIPTION. X data for features
        y : DataFrame
            DESCRIPTION. y data for target

        Returns
        -------
        None.

        """
        self.X = X
        self.y = y
        
        self._data_clean() # delete the features with constant values
        
        X = self.X.values
        y = self.y.values.flatten()
        
        self.selector.fit(X, y)

        self._new_df()
        self._rank_fts()
        
        self.selected_scores = self.scores[self.selector.get_support()]
        
    def _rank_fts(self):
        import numpy as np
        
        self.p_values = self.selector.pvalues_
        self.scores = self.selector.scores_
        self.scores /= self.scores.max()
        ranks = np.argsort(-self.scores)
        self.sorted_fts = self.X.columns[ranks]
        
    def _new_df(self):
        import pandas as pd
        
        X_new = self.selector.transform(self.X.values)
        self.selected_fts = self.X.columns[self.selector.get_support()]
        idxs = self.X.index
        
        self.X_sorted = pd.DataFrame(data=X_new, index=idxs, columns=self.selected_fts)      
    
    def plot_ft_scores(self, filename=None):
        """
        Plot the scores for each feature selected

        Parameters
        ----------
            filename : str
                DESCRIPTION. Path to file to save result to.

        Returns
        -------
        None.

        """
        import matplotlib.pyplot as plt
        # Trucante feature names
        features = [f.rsplit(".")[-1] for f in self.selected_fts]
        # Init figure
        fig, ax = plt.subplots()
        # Formatting
        ax.set_ylabel("Score", fontsize=14)
        # Plotting
        ax.bar(features, self.selected_scores, width=0.2)
        plt.xticks(rotation="vertical")
        plt.tight_layout()

        if filename is not None:
            # Make sure directory exists
            file_dir = filename[:filename.rfind("/")]
            if "/" in filename and not os.path.isdir(file_dir):
                os.makedirs(file_dir)
            fig.savefig(filename, bbox_inches="tight")
        else:
            plt.show()
        
    def write_scores_to_txt(self, filename):
        """
        Write all the scores and corresponding features into a txt file

        Parameters
        ----------
        filename : TYPE, optional
            DESCRIPTION. The path to the file to save results to.

        Returns
        -------
        None.

        """
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)

        # Open file
        fl = open(filename, "w")
        
        for i, ft in enumerate(self.X.columns[:-1]):
            fl.write(ft + ',' + str(self.scores[i]) + '\n')
        fl.write(self.X.columns[-1] + ',' + str(self.scores[-1]))
            
        fl.close()
    
    def write_sorted_features(self, filename):
        """
        Write the sorted features into a txt file

        Parameters
        ----------
        filename : TYPE, optional
            DESCRIPTION. The path to the file to save results to.


        Returns
        -------
        None.

        """
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)

        # Open file
        fl = open(filename, "w")

        for ft in self.sorted_fts[:-1]:
            fl.write(ft + '\n')
        fl.write(self.sorted_fts[-1])
            
        fl.close()
        
    
    def write_newdata_to_txt(self, filename):
        """
        Write the selected data into a txt file

        Parameters
        ----------
        filename : TYPE, optional
            DESCRIPTION. The path to the file to save results to.

        Returns
        -------
        None.

        """

        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)

        # Open file
        fl = open(filename, "w")
        
        for i, ft in enumerate(self.X_sorted.columns[:-1]):
           fl.write(ft + ',')
           for vl in self.X_sorted.values[:-1,i]:
               fl.write(str(vl) + ',')
           fl.write(str(self.X_sorted.values[-1,i]) + '\n')
           
        fl.write(self.X_sorted.columns[-1] + ',')
        for vl in self.X_sorted.values[:-1,i]:
             fl.write(str(vl) + ',')
        fl.write(str(self.X_sorted.values[-1,i]))
           
        fl.close()
        
    def get_data(self):
        """
        Return the selected data

        Returns
        -------
        dataset : DataFrame
            DESCRIPTION. feature data (X data)
        target : DataFrame
            DESCRIPTION. target data (y data)

        """
        X = self.X_sorted
        y = self.y
        return X, y
         

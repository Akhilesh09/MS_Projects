#!/usr/bin/env python3

# Inherit from these for Project.learner compatibility with gridsearchcv
from sklearn.base import BaseEstimator, ClassifierMixin

from sklearn.preprocessing import StandardScaler
from sklearn.preprocessing import RobustScaler
from sklearn.preprocessing import MaxAbsScaler
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import Normalizer
from sklearn.decomposition import PCA

import numpy as np
import pandas as pd

class LearnerBase(BaseEstimator, ClassifierMixin):
    """
    Base class for learners.
    """

    def __init__(self, data_opts):
        """
        Base class initializer

        Parameters
        ----------
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for information on valid inputs.

        Returns
        -------
        None.

        """
        
        self.data_opts = data_opts

    def fit(self, X, y):
        """
        Fit the model to input data X and classifications y.
        This method is meant to mimic the structure of the Scikit-Learn models.
        
        Parameters 
        ----------
        X : pandas.DataFrame
            2D dataframe containing the training data.
        y : numpy.ndarray
            1D array containing the correct classifications for X.
        """

        # Data processing
        
        self.preproc = self.data_transormation(X, **self.data_opts)
        X = self.transform(X)
        
        self.learner_.fit(X, y)
    
    
    def score(self, X, y):
        """
        Score the model on test data (X,y). This method is meant 
        to mimic the structure of the Scikit-Learn models.
        
        Parameters 
        ----------
        X : pandas.DataFrame
            2D dataframe containing the data for which to make predictions.
        y : numpy.ndarray
            1D array containing the correct classifications for X.
            
        Returns
        -------
        float
            Accuracy of the model on test set (X, y)
        """
        
        # Apply transformations on test data
        X = self.transform(X)
        
        return self.learner_.score(X, y)

    def predict(self, X):
        """
        Predict labels using the model on test data X. This method is meant 
        to mimic the structure of the Scikit-Learn models.
        
        Parameters 
        ----------
        X : pandas.DataFrame
            2D dataframe containing the data for which to make predictions.
        
        Returns
        -------
        float
            Predictions of the model on test set X
        """
        
        # Apply transformations on test data
        X = self.transform(X)
        
        return self.learner_.predict(X)

    ''' sklearn's GridSearchCV clones the current class object before changing
    the values of the attributes passed directly into __init__. By creating the
    copy first, any dependent learners (i.e., self.learner_) are initialized with
    the old hyperparameters. Thus, the set_params function must be overridden
    to re-initialize these learners using the updated hyperparameters.
    '''
    
    def set_params(self, **params):
        super().set_params(**params)
        self.reinit()
        return self
    
    def reinit(self):
        raise NotImplementedError("reinit must be implemented in the derived class.")
    
    ###########################################################################
    ##################### Data transformation functions #######################
    ###########################################################################

    def transform(self, X):
        """
        Transform the samples in index list ind using the preprocessor
        or preprocessor dictionary.

        Params
        ------
        X : pandas.DataFrame
            nsample x nfeature dataframe of inputs to transform.

        Returns
        -------
        Transformed data-set according to preprocessors.
        """
        
        # If a single preprocessor, easy
        if not isinstance(self.preproc, dict):
            return self.preproc.transform(np.asarray(X))
        # If a dictionary of preprocessors, a little tough
        else:
            data = []
            programs = X.index
            # Sequentially transform each entry according to 
            # the preprocessor with the same time stamp
            for i in range(X.shape[0]):
                # Get the program name for this index
                program = programs[i]
                # Split the program by "-" to get stamp
                tmp = program.split("-")
                for entry in tmp:
                    if entry.isdigit():
                        # Get data for program i
                        x = X.iloc[i][np.newaxis, :]
                        # Ensure a preprocessor exist for this stamp
                        if entry not in self.preproc.keys():
                            msg = "No preprocessor found for stamp {}."
                            raise KeyError(msg.format(entry))
                        # Transform program i data with correct preprocessor
                        x = self.preproc[entry].transform(x)
                        # Add to vector
                        data.append(x.ravel())
            return np.atleast_2d(data)
                
    def data_transormation(self, X, pca_opt=False, pca_threshold=0.99,
                           norm_opt=None, domain_opt="global"):
        """
        Data transformations tool.
        Params
        ------
        X : pandas.DataFrame
            2D dataframe containing the training data.
        pca_opt : Boolean for PCA use.
        pca_threshold : Fraction of explained variance to keep.
        norm_opt : max / standard / robust / minmax / norm / None
            max = Divide by maximum observed value.
            standard = Remove mean, divide by standard deviation.
            robust = Remove median, divide by inter-quartile range
            minmax = Map to (min, max) range. Defined to be (0, 1)
            norm = Normalize each each sample to unit L2 norm.
            None = no data transformation
        domain_opt : global / time.
        
        Returns
        -------
        Scikit-Learn object containing normalization information.
        """

        # If no pca or normalization, return filtered data
        if not pca_opt and norm_opt is None:
            
            # Dumb class for no transformation
            class no_transformation:
                def transform(self, X):
                    return X
            
            return no_transformation()

        # Perform PCA
        if pca_opt:
            # whiten = apply standard normalization
            pca = PCA(n_components=pca_threshold, whiten=True)
            pca.fit(X)
            return pca

        # Perform normalization
        else:
            if domain_opt == "time":
                return self.preprocess_per_stamp(X, norm_opt)
            
            elif domain_opt == "global":
                
                X = np.asarray(X)
                
                if norm_opt == "max":
                    preproc = MaxAbsScaler()
                    preproc.fit(X)

                elif norm_opt == "norm":
                    preproc = Normalizer()
                    preproc.fit(X)

                elif norm_opt == "minmax":
                    preproc = MinMaxScaler(feature_range=(0,1))
                    preproc.fit(X)

                elif norm_opt == "standard":
                    preproc = StandardScaler()
                    preproc.fit(X)
                
                elif norm_opt == "robust":
                    preproc = RobustScaler()
                    preproc.fit(X)
                
                else:
                    msg = "{} preprocessor not valid."
                    raise ValueError(msg.format(norm_opt))
                
                return preproc

    def preprocess_per_stamp(self, X, norm_opt):
        """
        Prepare preprocessors for each stamp.
        """

        programs = X.index
        
        # Get stamps
        stamps = self.get_unique_time_stamps(programs)

        # Init storage for preprocessors
        preprocs = {}
        
        # Iterate through stamps
        for stamp in stamps:
            # Find each program with this stamp
            progs = []
            for i in range(X.shape[0]):
                program = programs[i]
                tmp = program.split("-")
                for entry in tmp:
                    if entry == stamp:
                        progs.append(program)
            # Get data for these programs
            x = X.loc[progs].values # FIXME
            # Initialize preprocessor
            if norm_opt == "max":
                preproc = MaxAbsScaler()
                preproc.fit(x)
                preprocs[stamp] = preproc

            elif norm_opt == "norm":
                preproc = Normalizer()
                preproc.fit(x)
                preprocs[stamp] = preproc

            elif norm_opt == "minmax":
                preproc = MinMaxScaler((0,1))
                preproc.fit(x)
                preprocs[stamp] = preproc

            elif norm_opt == "standard":
                preproc = StandardScaler()
                preproc.fit(x)
                preprocs[stamp] = preproc
                
            elif norm_opt == "robust":
                preproc = RobustScaler()
                preproc.fit(x)
                preprocs[stamp] = preproc
            else:
                msg = "{} preprocessor not valid."
                raise ValueError(msg.format(norm_opt))
        return preprocs

    def get_unique_time_stamps(self, programs):
        """
        Get a unique list of time stamps from the training set.
        """

        # Init empty list
        stamps = []
        for program in programs:
            tmp = program.split("-")
            for entry in tmp:
                if entry.isdigit():
                    if entry not in stamps:
                        stamps.append(entry)
        return stamps
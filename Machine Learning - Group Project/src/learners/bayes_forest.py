#!/usr/bin/env python3

import numpy as np
import pandas as pd
from learners.bayes import NaiveBayes
from learners.perceptron import Perceptron
from sklearn.ensemble import BaggingClassifier
from learners.learner_base import LearnerBase
import inspect

class BayesForest(LearnerBase):
    """
    Class for Bayes forest model.
    """
    
    def __init__(self,
                 # Naive Bayes params
                 priors=None, var_smoothing=1e-9,
                 # Perceptron params
                 eta0=0.01, max_iter=1000, tol=1E-6, random_state_perceptron=4,
                 penalty=None, alpha=0.0001, fit_intercept=True, shuffle=True,
                 verbose_perceptron=0, n_jobs_perceptron=None, early_stopping=False,
                 validation_fraction=0.1, n_iter_no_change=5, class_weight_perceptron=None,
                 warm_start_perceptron=False,
                 # Bagging params
                 n_estimators=10, max_samples=0.1, max_features_bagger=1.0,
                 bootstrap=True, bootstrap_features=False, oob_score=False,
                 warm_start_bagger=False, n_jobs_bagger=None, random_state_bagger=3,
                 verbose_bagger=0,
                 data_opts={}):
        """
        Class initializer.

        Params
        ------
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        
        See Scikit-Learn documentation.
        """

        args, _, _, values = inspect.getargvalues(inspect.currentframe())
        values.pop("self")
    
        for arg, val in values.items():
            setattr(self, arg, val)
        
        # Split parameters into their respective learners
        
        self.bayes_param_names = ["priors", "var_smoothing"]
        self.bagger_param_names = ["n_estimators", "max_samples", "max_features",
                                   "bootstrap", "bootstrap_features", "oob_score",
                                   "warm_start", "n_jobs",
                                   "random_state_perceptron", "verbose"]
        self.perceptron_param_names = ["eta0", "max_iter", "tol", "random_state_perceptron",
                                       "penalty", "alpha", "fit_intercept",
                                       "shuffle", "n_jobs", "early_stopping",
                                       "n_iter_no_change", "class_weight"]
        
        # Set up dependent learners
        self.reinit()
        
    def reinit(self):
        # Get most current parameter values
        all_params = self.get_params()
        super().__init__(all_params["data_opts"])
        bayes_params = {name : value for name, value in all_params.items()
                     if name in self.bayes_param_names}
        bagger_params = {name.split("_bagger")[0] : value for name, value in all_params.items()
                         if name in self.bagger_param_names}
        perceptron_params = {name.split("_perceptron")[0] : value for name, value in all_params.items()
                             if name in self.perceptron_param_names}
        
        
        # Init weak learner base class
        self.base_ = NaiveBayes(**bayes_params)
        # Init collection of weak learners
        self.weak_learners_ = BaggingClassifier(**bagger_params)
        # Init perceptron aggregator
        self.learner_ = Perceptron(**perceptron_params)

    def fit(self, X, y): 
        """
        Fit the model to input data (X,y). This method is meant 
        to mimic the structure of the Scikit-Learn models.
        
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
        
        # Train all decision trees
        self.weak_learners_.fit(X, y)
        # Get perceptron input
        Xdt = self.build_perceptron_input(X)
        # Fit perceptron model
        self.learner_.fit(Xdt, y)

    def predict(self, X):
        """
        Predict the class based on input data X.
        This method is meant to mimic the structure of the Scikit-Learn models.
        
        Parameters 
        ----------
        X : numpy.ndarray
            2D array containing the training data.
        """
        X = self.transform(X)
        X = self.build_perceptron_input(X)
        return self.learner_.predict(X)       

    def score(self, X, y):
        """
        Score the model on test data (X,y). This method is meant 
        to mimic the structure of the Scikit-Learn models.
        
        Parameters 
        ----------
        X : numpy.ndarray
            2D array containing the training data.

        y : numpy.ndarray
            1D array containing the correct classifications for X.
            
        Returns
        -------
        float
            Accuracy of the perceptron forest model on test set (X, y)
        """
        X = self.transform(X)
        X = self.build_perceptron_input(X)
        return self.learner_.score(X, y)

    def decision_function(self, X):
        """
        Predict confidence scores for samples. The confidence score for a sample
        is the signed distance of that sample to the hyperplane. This method is
        meant to mimic the structure of the Scikit-Learn models.

        Parameters
        ----------
        X : numpy.ndarray
            2D array containing the training data.

        Returns
        -------
        numpy.ndarray
            Confidence scores for samples.

        """
        X = self.transform(X)
        X = self.build_perceptron_input(X)
        return self.learner_.decision_function(X)

    def build_perceptron_input(self, X):
        """
        Construct the inputs for the perceptron in the perceptron forest model
        by making predictions with the collection of decision trees.
        
        Parameters
        ----------
        X : numpy.ndarray
            2D array containing the training data.
            
        Return
        ------
        X_bayes : numpy.ndarray
            2D array containing the prediction of each Baye's estimator for 
            each training example in X.

        """
        # Init perceptron input
        X_bayes = np.zeros((X.shape[0], self.n_estimators))
        
        # Build perceptron input
        estimators = self.weak_learners_.estimators_
        for i, estimator in enumerate(estimators):
            X_bayes[:,i] = estimator.predict(X)
        return X_bayes

    def get_CV_info(self, X, y):
        pass


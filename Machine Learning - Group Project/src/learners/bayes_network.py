#!/usr/bin/env python3

import numpy as np
import pandas as pd
from learners.bayes import NaiveBayes
from learners.neural_network import NeuralNetwork
from sklearn.ensemble import BaggingClassifier
from learners.learner_base import LearnerBase
import inspect

class BayesNetwork(LearnerBase):
    """
    Class for Bayes network model.
    """

    def __init__(self,
                 # Naive Bayes params
                 priors=None, var_smoothing=1e-9,
                 # Neural net params
                 hidden_layer_sizes=(100, ), activation='relu', solver='adam',
                 learning_rate='constant', momentum=0.9, batch_size='auto',
                 random_state_net=3, alpha=0.0001, learning_rate_init=0.001,
                 power_t=0.5, max_iter=200, shuffle=True, tol=0.0001, verbose=False,
                 warm_start=False, nesterovs_momentum=True, early_stopping=False,
                 validation_fraction=0.1, beta_1=0.9, beta_2=0.999, epsilon=1e-08,
                 n_iter_no_change=10, max_fun=15000,
                 # Bagging params
                 n_estimators=10, max_samples=0.1, max_features_bagger=1.0,
                 bootstrap=True, bootstrap_features=False, oob_score=False,
                 warm_start_bagger=False, n_jobs_bagger=None, random_state_bagger=3,
                 verbose_bagger=0,
                 data_opts={}):
        """
        Class initializer.

        Parameters
        ----------
        
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        others: see sklearn documentation.
        
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
        self.neural_net_param_names = ['hidden_layer_sizes', 'activation',
                                       'solver', 'learning_rate', 'momentum',
                                       'batch_size', 'random_state_net', 'alpha',
                                       'learning_rate_init', 'power_t', 'max_iter',
                                       'shuffle', 'tol', 'verbose', 'warm_start',
                                       'nesterovs_momentum', 'early_stopping',
                                       'validation_fraction', 'beta_1', 'beta_2',
                                       'epsilon', 'n_iter_no_change', 'max_fun']
        
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
        neural_network_params = {name.split("_net")[0] : value for name, value in all_params.items()
                             if name in self.neural_net_param_names}
        
        
        # Init weak learner base class
        self.base_ = NaiveBayes(**bayes_params)
        # Init collection of weak learners
        self.weak_learners_ = BaggingClassifier(base_estimator=self.base_, **bagger_params)
        # Init neural net aggregator
        self.learner_ = NeuralNetwork(**neural_network_params)

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
        
        # Train all bayes learners
        self.weak_learners_.fit(X, y)
        # Get neural net input 
        Xb = self.build_neural_net_input(X)
        # Fit neural net model
        self.learner_.fit(Xb, y)

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
        X = self.build_neural_net_input(X)
        return self.learner_.predict(X)

    def score(self, X, y):
        """
        Fit the model on test data (X,y). This method is meant 
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
        X = self.build_neural_net_input(X)
        return self.learner_.score(X, y)      

    def build_neural_net_input(self, X):
        """
        Construct the inputs for the neural net in the ensemble network model
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
        # Init neural net input
        X_bayes = np.zeros((X.shape[0], self.n_estimators))
        
        # Build neural net input
        estimators = self.weak_learners_.estimators_
        for i, estimator in enumerate(estimators):
            X_bayes[:,i] = estimator.predict(X)
        return X_bayes

    def get_CV_info(self, X, y):
        pass




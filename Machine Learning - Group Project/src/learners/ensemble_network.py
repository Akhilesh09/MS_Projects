#!/usr/bin/env python3

import numpy as np
import pandas as pd
from learners.decision_tree import DecisionTree
from learners.neural_network import NeuralNetwork
from sklearn.ensemble import BaggingClassifier
from learners.learner_base import LearnerBase
import inspect


class EnsembleNetwork(LearnerBase):
    """
    Class for ensemble network model.
    """
    
    def __init__(self,
                 # Decision tree params
                 max_depth=None, criterion='entropy', ccp_alpha=0.0,
                 random_state_tree=3, splitter='best', min_samples_split=2,
                 min_samples_leaf=1, min_weight_fraction_leaf=0.0,
                 max_features_tree=None, max_leaf_nodes=None, min_impurity_decrease=0.0,
                 min_impurity_split=None, class_weight_tree=None,
                 # Neural net params
                 hidden_layer_sizes=(4,2), activation='relu', solver='adam',
                 learning_rate='constant', momentum=0.9, batch_size='auto',
                 random_state_net=3, alpha=0.0001, learning_rate_init=0.001,
                 power_t=0.5, max_iter=int(1E6), shuffle=True, tol=0.0001, verbose=False,
                 warm_start=False, nesterovs_momentum=True, early_stopping=False,
                 validation_fraction=0.1, beta_1=0.9, beta_2=0.999, epsilon=1e-08,
                 n_iter_no_change=10, max_fun=15000,
                 # Bagging params
                 n_estimators=6, max_samples=0.1, max_features_bagger=1.0,
                 bootstrap=True, bootstrap_features=False, oob_score=False,
                 warm_start_bagger=False, n_jobs_bagger=None, random_state_bagger=3,
                 verbose_bagger=0,
                 data_opts={}):
        """
        Class initializer.
        
        Parameters
        ----------
        DT hyperparms = see DecisionTree class.
        Neural netwok hyperparams = see Neural Network class.
        n_estimators = number of weak learners to use (default: 10)
        max_samples  = number or fraction of samples to train weak learners with
        replacement  = whether to sample training data for weak learners with replacement
        random_state_bagger : int, optional
            random seed to use for shuffling. The default is 3.
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        others: see sklearn.ensemble.BaggingClassifier documentation.
        """

        # Set the hyperparams
        # The below takes each input parameter and defines self.parameter = parameter
        
        args, _, _, values = inspect.getargvalues(inspect.currentframe())
        values.pop("self")
    
        for arg, val in values.items():
            setattr(self, arg, val)
        
        # Split parameters into their respective learners
        
        self.DT_param_names = ["max_depth", "criterion", "ccp_alpha",
                               "random_state_tree", "splitter", "min_samples_split",
                               "min_samples_leaf", "min_weight_fraction_leaf",
                               "max_features", "max_leaf_nodes", "min_impurity_decrease",
                               "min_impurity_split", "class_weight"]
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
        DT_params = {name.split("_tree")[0] : value for name, value in all_params.items()
                     if name in self.DT_param_names}
        bagger_params = {name.split("_bagger")[0] : value for name, value in all_params.items()
                         if name in self.bagger_param_names}
        neural_net_params = {name.split("_net")[0] : value for name, value in all_params.items()
                             if name in self.neural_net_param_names}
        
        # Init weak learner base class
        self.base_ = DecisionTree(**DT_params)
        
        # Init collection of weak learners
        self.weak_learners_ = BaggingClassifier(base_estimator=self.base_, **bagger_params)
        
        # Init Neural Network aggregator
        self.learner_ = NeuralNetwork(**neural_net_params)

    
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
        # Get neural net input
        Xdt = self.build_neural_net_input(X)
        # Fit neural net model
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
            Accuracy of the neural network model forest model on test set (X, y)
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
        X_DT : numpy.ndarray
            2D array containing the prediction of each decision tree for 
            each training example in X.

        """
        # Init neural net input
        X_DT = np.zeros((X.shape[0], self.n_estimators))
        
        # Build neural net input
        estimators = self.weak_learners_.estimators_
        for i, estimator in enumerate(estimators):
            X_DT[:,i] = estimator.predict(X)
        return X_DT

    def get_CV_info(self, X, y):
        """
        Placeholder for compatibility.
        """
        pass
    
    def write_CV_learner(self, CV_learner, ID, labels, output_dir):
        """
        Placeholder for compatibility.
        """
        pass

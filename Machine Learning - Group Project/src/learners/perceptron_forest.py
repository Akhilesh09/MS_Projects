#!/usr/bin/env python3

import numpy as np
import pandas as pd
from learners.decision_tree import DecisionTree
from learners.perceptron import Perceptron
from sklearn.ensemble import BaggingClassifier
from learners.learner_base import LearnerBase
import inspect


class PerceptronForest(LearnerBase):
    """
    Class for perceptron forest model.
    """
    
    def __init__(self,
                 # Decision tree params
                 max_depth=None, criterion='entropy', ccp_alpha=0.0,
                 random_state_tree=3, splitter='best', min_samples_split=2,
                 min_samples_leaf=1, min_weight_fraction_leaf=0.0,
                 max_features_tree=None, max_leaf_nodes=None, min_impurity_decrease=0.0,
                 min_impurity_split=None, class_weight_tree=None,
                 # Perceptron params
                 eta0=0.01, max_iter=1000, tol=1E-6, random_state_perceptron=3,
                 penalty=None, alpha=0.0001, fit_intercept=True, shuffle=True,
                 verbose_perceptron=0, n_jobs_perceptron=None, early_stopping=False,
                 validation_fraction=0.1, n_iter_no_change=5, class_weight_perceptron=None,
                 warm_start_perceptron=False,
                 # Bagging params
                 n_estimators=2, max_samples=0.2, max_features_bagger=1.0,
                 bootstrap=True, bootstrap_features=False, oob_score=False,
                 warm_start_bagger=False, n_jobs_bagger=None, random_state_bagger=3,
                 verbose_bagger=0,
                 data_opts={}):
        """
        Class initializer.
        
        Parameters
        ----------
        DT hyperparms = see DecisionTree class.
        Perceptron hyperparams = see Perceptron class.
        n_estimators = number of weak learners to use (default: 2)
        max_samples  = number or fraction of samples to train weak learners
            with (defualt 0.2).
        replacement  = whether to sample training data for weak learners with
            replacement (default True).
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
        self.perceptron_param_names = ["eta0", "max_iter", "tol", "random_state_bagger",
                                       "penalty", "alpha", "fit_intercept",
                                       "shuffle", "n_jobs", "early_stopping",
                                       "n_iter_no_change", "class_weight"]
        
        # Set up dependent learners
        self.reinit()
        
    def reinit(self):
        
        # Get most current parameter values
        all_params = self.get_params()
        super().__init__(all_params["data_opts"])
        DT_params = {name.split("_tree")[0] : value for name, value in all_params.items()
                     if name in self.DT_param_names}
        bagger_params = {name.split("_perceptron")[0] : value for name, value in all_params.items()
                         if name in self.bagger_param_names}
        perceptron_params = {name.split("_bagger")[0] : value for name, value in all_params.items()
                             if name in self.perceptron_param_names}
        
        # Init weak learner base class
        self.base_ = DecisionTree(**DT_params)
        
        
        # Init collection of weak learners
        self.weak_learners_ = BaggingClassifier(base_estimator=self.base_, **bagger_params)
        
        
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
        
        Xdt = self.build_perceptron_input(X)
        return self.learner_.predict(Xdt)       
        
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
        
        Xdt = self.build_perceptron_input(X)
        return self.learner_.score(Xdt, y)
    

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


    def get_CV_info(self, X, y):
        """
        Get information from a cross-validation learner instance for 
        a given set of test data (X,y).
        
        Parameters
        ----------
        X : numpy.ndarray
            2D array containing the test data.

        y : numpy.ndarray
            1D array containing the test labels.

        Return
        ------
        weights : numpy.ndarray
            1D array of perceptron weight in order corresponding to the 
            features in X.

        (y, y_score) : (numpy.ndarray, numpy.ndarray)
            1D vectors of the actual correct classifications and the 
            score for each test according to the decision function.

        trees : (sklearn.tree.DecisionTreeClassifier)
            List of decision trees in ensemble.
        """

        # Copy perceptron weights and bias into data frame
        weights = np.append(self.learner_.coef_.flatten(),
                            self.learner_.intercept_.flatten())
        
        # Get confidence scores on test data for ROC curve
        y_score = self.decision_function(X)
        
        # Get trees from forest
        trees = [tree.learner_ for tree in self.weak_learners_.estimators_]
        return weights, (y, y_score), trees


    def write_CV_learner(self, CV_learner, ID, labels, output_dir):
        """
        Write information about this learner to files.

        Parameters
        ----------
        CV_learner : (numpy.ndarray, (numpy.ndarray, numpy.ndarray), 
                     (sklearn.tree.DecisionTreeClassifier))
            Information provided from get_CV_info.

        ID : int or str
            Some label associated with this learner.

        labels : (str)
            List of strings corresponding to feature names
        
        output_dir : str
            Directory to which to write output files.
        """

        # Form outfile
        outfile = "{}/PF_{}".format(output_dir, ID)

        # Form perceptron weights DataFrame
        indices = ["PFDT{}".format(i) for i in range(self.n_estimators)] + ["bias"]
        DF = pd.DataFrame(CV_learner[0], index=indices, columns=['weight'])

        # Write perceptron info to files
        self.learner_.write_perceptron_weights(DF, outfile)
        
        # Generate ROC curve
        bias = self.learner_.intercept_.item()
        self.learner_.generate_ROC_curve(CV_learner[1], bias, outfile)
        
        # Write tree info to files
        for i, DT in enumerate(CV_learner[2]):
            outfile = "{}/PF_DT{}_{}".format(output_dir, i, ID)
            self.base_.write_DT(DT, outfile, labels, False)
            
    
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
        Xdt : numpy.ndarray
            2D array containing the prediction of each decision tree for 
            each training example in X.

        """
        # Init perceptron input
        X_DT = np.zeros((X.shape[0], self.n_estimators))
        
        # Build perceptron input
        estimators = self.weak_learners_.estimators_
        for i, estimator in enumerate(estimators):
            X_DT[:,i] = estimator.predict(X)
        return X_DT

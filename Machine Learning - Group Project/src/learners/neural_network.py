#!/usr/bin/env python3

import numpy as np
from copy import deepcopy
from sklearn.neural_network import MLPClassifier
from learners.learner_base import LearnerBase
import inspect

class NeuralNetwork(LearnerBase):
    """
    Wrapper class for scikit-learn MLPClassifier
    """

    def __init__(self, hidden_layer_sizes=(6,6,6), activation='relu', solver='adam',
                 learning_rate='constant', momentum=0.9, batch_size=200,
                 random_state=3, alpha=0.0001, learning_rate_init=0.001,
                 power_t=0.5, max_iter=2000, shuffle=True, tol=0.0001, verbose=False,
                 warm_start=False, nesterovs_momentum=True, early_stopping=False,
                 validation_fraction=0.1, beta_1=0.9, beta_2=0.999, epsilon=1e-08,
                 n_iter_no_change=10, max_fun=15000, data_opts={}):
        """
        Class initializer.

         Parameters
        ----------
        hidden_layer_sizes : tuple, (default: (100,))
            Number of perceptrons in each hidden layer.

        activation : str ('identity', 'logistic', 'tanh' or 'relu')
            (default: 'relu')
            String for the type of activation function to use.
            'identity': g(x) = x 
            'logistic': g(x) = 1 / (1 + exp(-x))
            'tanh': g(x) = tanh(x)
            'relu': g(x) = max(0, x)
        
        solver : str ('lbfgs', 'sgd', 'adam'), (default: 'adam')
            The algorithm that solves for the weights.
            'lbfgs': quasi-Newton optimizer
            'sgd': stochastic gradient descend
            'adam': improved stochastic gradient descend

        learning_rate : str ('constant', 'invscaling', 'adaptive')
            (default: 'constant')
            'constant': self explanatory
            'invscaling': decreases learning rate with the inverse of the
                          iteration number (time step)
            'adaptive': decreases learning rate by a fator of 5 
                        if the loss does not change

        momentum : float, (default: 0.9)
            The momentum in the regular stochastic gradient algorithm.
            Used if 'sgd' is set in solver.

        batch_size : int (default: 'auto')
            The size of the mini-batches for stochastic gradient descend
            algorithms.
            'auto' is min(200, n_samples) in this case.
                
        random_state : int, optional
            Seed used for random shuffling. The default is 3.
        
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        
        others: see sklearn.neural_network.MLPClassifier documentation.
        """
        
        # Set the hyperparams
        # The below takes each input parameter and defines self.parameter = parameter
        
        args, _, _, values = inspect.getargvalues(inspect.currentframe())
        values.pop("self")
    
        for arg, val in values.items():
            setattr(self, arg, val)

        # Init learning model
        self.reinit()
        
    def reinit(self):
        params = self.get_params()
        super().__init__(params["data_opts"])
        del params["data_opts"]
        # Init neural net learner, pass in all parameters passed to self.__init__
        self.learner_ = MLPClassifier(**params)

    def get_CV_info(self, X, y):
        """
        Placeholder for compatibility.
        """
        pass
    
    def write_CV_learner(self, CV_learner, ID, labels, output_dir):
        """
        Function for compatibility.
        """
        pass
         

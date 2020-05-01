#!/usr/bin/env python3

import numpy as np
import pandas as pd
from learners.learner_base import LearnerBase
from sklearn.naive_bayes import GaussianNB
from copy import deepcopy
import inspect

class NaiveBayes(LearnerBase):
    """
    Wrapper function for scikit-learn NaiveBayes.
    """

    def __init__(self, priors=None, var_smoothing=1e-9, data_opts={}):
        """
        Class initializer.

        Params
        ------
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        
        See Scikit-Learn documentation for Naive Bayes learners.
        """

        # Set the hyperparams
        # The below takes each input parameter and defines self.parameter = parameter
        
        args, _, _, values = inspect.getargvalues(inspect.currentframe())
        values.pop("self")
    
        for arg, val in values.items():
            setattr(self, arg, val)

        # Set up dependent learners
        self.reinit()

    def reinit(self):
        params = self.get_params()
        super().__init__(params["data_opts"])
        del params["data_opts"]
        # Init decision tree learner, pass in all parameters passed to self.__init__
        self.learner_ = GaussianNB(**params)

    def get_CV_info(self, X, y):
        return deepcopy(self.learner_)

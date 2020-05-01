#!/usr/bin/env python3

import numpy as np
from copy import deepcopy
from sklearn.neighbors import KNeighborsClassifier
from learners.learner_base import LearnerBase
import inspect

class KNN(LearnerBase):
    """
    Wrapper class for scikit-learn KNeighborsClassifier
    """

    def __init__(self, n_neighbors=1, weights='dist_sq', algorithm='brute',
                 leaf_size=30, p=2, metric='minkowski', metric_params=None,
                 n_jobs=None, data_opts={}):
        """
        Class initializer.

         Parameters
        ----------
        n_neighbors : int (default: 1)
            Number of neighbors to use.

        weights : str ('uniform', 'distance', or 'dist_sq') (default 'dist_sq')
            String for the type of weighting to use.
            'uniform' is self explanatory. 'distance' weights 
            examples by the inverse of the distance measure.
            'dist_sq' weights neighbors by the inverse of the distance squared.
        
        algorithm : str ('brute' or 'ball_tree') (default 'brute')
            The algorithm used for querying the model.
        
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        
        others: see parameters to sklearn's sklearn.neighbors.KNeighborsClassifier.
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
        # Init knn learner, pass in all parameters passed to self.__init__
        
        # Option for weighting neighbor votes by inverse squared distance
        if self.weights == "dist_sq":
            def weight_function(distances):
                
                # If a distance is 0, that point's weight is 1 and all others are 0.
                with np.errstate(divide='ignore'):
                    weights = 1. / distances**2
                
                inf_mask = np.isinf(weights)
                inf_row = np.any(inf_mask, axis=1)
                weights[inf_row] = inf_mask[inf_row]

                return weights
            
            self.weights = weight_function
        
        params = self.get_params()
        super().__init__(params["data_opts"])
        del params["data_opts"]
        
        self.learner_ = KNeighborsClassifier(**params)

    def get_CV_info(self, X, y):
        """
        Placeholder for compatibility.
        """
        pass
    
    def write_CV_learner(self, CV_learner, ID, labels, output_dir):
        pass
         

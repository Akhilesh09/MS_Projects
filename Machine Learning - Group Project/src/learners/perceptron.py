#!/usr/bin/env python3

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import sklearn.linear_model as sklm
from sklearn.metrics import auc
from learners.learner_base import LearnerBase
import inspect

class Perceptron(LearnerBase):

    """
    Wrapper class for scikit-learn perceptron.
    """
    
    def __init__(self, eta0=0.05, max_iter=1000, tol=1E-6, random_state=3,
                 penalty=None, alpha=0.0001, fit_intercept=True, shuffle=True,
                 verbose=0, n_jobs=None, early_stopping=False,
                 validation_fraction=0.1, n_iter_no_change=5, class_weight=None,
                 warm_start=False, data_opts={}):
        """
        Class initializer.

        Parameters
        ----------
        eta0     = learning rate (default: 0.1)
        max_iter = maximum passes through training data (default: 1000)
        tol      = convergence tolerance for weights (default: 1e-6)
        random_state : int, optional
            Seed for random shuffling. The default is 3.
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
        
        others: see parameters to sklearn's sklearn.linear_model.Perceptron.
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
        # Init decision perceptron learner, pass in all parameters passed to self.__init__
        self.learner_ = sklm.Perceptron(**params)

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
        """

        # Copy perceptron weights and bias into data frame
        weights = np.append(self.coef_.flatten(),
                            self.intercept_.flatten())

        # Get confidence scores on test data for ROC curve
        y_score = self.learner_.decision_function(X)
        
        return weights, (y, y_score)

    
    def write_CV_learner(self, CV_learner, ID, labels, outdir):
        """
        Write information about this learner to files.

        Parameters
        ----------
        CV_learner : (numpy.ndarray, (numpy.ndarray, numpy.ndarray), 
            Information provided from get_CV_info.

        ID : int or str
            Some label associated with this learner.

        labels : (str)
            List of strings corresponding to feature names
        
        outdir : str
            Directory to which to write output files.
        """

        # Form path for output file
        outfile = "{}/perceptron_{}".format(outdir, ID)

        # Form perceptron weights DataFrame
        indices = np.append(labels, ["bias"])
        DF = pd.DataFrame(CV_learner[0], index=indices, columns=["weight"])

        # Write the perceptron weights 
        self.write_perceptron_weights(DF, outfile)

        # Generate ROC curve
        bias = self.intercept_.item()
        self.generate_ROC_curve(CV_learner[1], bias, outfile)


    @staticmethod
    def write_perceptron_weights(DF, outfile):
        """
        Writes perceptron values and absolute values to file.

        Parameters
        ----------
        DF : pandas.DataFrame
            Data frame containing perceptron weights.
        outfile : str
            Path to file for output.
        """

        # sorting features from largest to smallest weight

        DF.sort_values(by=["weight"], ascending=False).to_csv(
            "{}-sorted.txt".format(outfile),
            sep=",",
            float_format="%.4e",
            index_label="features"
        )

        # sort by absolute value of weights
        DF.abs().sort_values(by=["weight"], ascending=False).to_csv(
            "{}-sorted-absolute.txt".format(outfile),
            sep=",",
            float_format="%.4e",
            index_label="features"
        )


    @staticmethod
    def generate_ROC_curve(y_data, bias, outfile):
        """
        Generate ROC curve for a perceptron.

        Parameters
        ----------
        y_data : tuple
            tuple containing true classifications and confidence scores of test data.
        bias : float
            Bias of the perceptron that produces confidence scores.
        outfile : str
            Path to file for output.
        """
        
        # Get false positive rate, true positive rate, and thresholds.
        
        y_true, y_scores = y_data
        
        num_positive = (y_true > 0).sum()
        num_negative = y_true.size - num_positive
        
        if num_positive == 0 or num_negative == 0:
            raise ValueError("All classifications are the same. Raising error to avoid division by 0.")
        
        # Placeholders for true and false positive rates and thresholds
        tpr, fpr, thresholds = [], [], []

        # Loop through confidence scores
        for score in np.append(0.0, y_scores):
            # Set bias to other side of example i
            thresholds.append(bias - (1.0 + 1e-6) * score)
            
            new_scores = y_scores - (1.0 + 1e-6) * score
            
            # Compute true positive rate
            tp = ((y_true > 0) & (new_scores > 0)).sum()
            tpr.append(tp/num_positive)
            
            # Compute false positive rate
            fp = ((y_true < 0) & (new_scores > 0.0)).sum()
            fpr.append(fp/num_negative)
        
        tpr = np.array(tpr)
        fpr = np.array(fpr)
        thresholds = np.array(thresholds)
        
        # Sort
        ind = np.argsort(tpr)
        
        tpr = tpr[ind]
        fpr = fpr[ind]
        thresholds = thresholds[ind]
        
        ind = np.argsort(fpr)

        tpr = tpr[ind]
        fpr = fpr[ind]
        thresholds = thresholds[ind]
        
        # Get area under curve
        area = auc(fpr, tpr)
        
        # Delete superfluous points
        points = []
        for x, y in zip(fpr, tpr):
            point = [x,y]
            if point not in points:
                points.append(point)
        points = np.array(points)
        fpr = points[:,0]; tpr = points[:,1]
        
        ind = np.argwhere(np.diff(tpr) > 0) + 1
        ind = np.append(0, ind)
        if ind[-1] < tpr.size - 1:
            ind = np.append(ind, tpr.size - 1)
        ind = np.append(ind, np.argwhere(np.diff(fpr) > 0) + 1)

        ind_fpr = np.argwhere(fpr == 0).flatten()[1:-1]

        # Remove ind_fpr from ind_tpr
        ind = np.array([value for value in ind if value not in ind_fpr])
        ind = np.unique(np.sort(ind))
        
        tpr = tpr[ind]
        fpr = fpr[ind]
        thresholds = thresholds[ind]
        
        # plot
        fig, ax = plt.subplots()
        ax.set_xlabel("False Positive Rate")
        ax.set_ylabel("True Positive Rate")
        ax.spines["right"].set_visible(False)
        ax.spines["top"].set_visible(False)
        
        x_min = min(fpr.min(), tpr.min())
        x_max = max(fpr.max(), tpr.max())
        
        ax.plot([x_min, x_max], [x_min, x_max], "--", label="Chance", color="black")
        ax.plot(fpr, tpr, "-o", label="AUC = {:.4f}".format(area), color="blue")
        
        # Compute distance between points
        d = np.sqrt(np.diff(tpr)**2 + np.diff(fpr)**2)
        cluster_groups = [] # List to hold indicies of close together points
        cluster_inds = []
        cluster = []
        for i in range(len(d)+1):
            if i < len(d) and d[i] < 5E-2:
                cluster.append(i)
                cluster_inds.append(i)
            elif len(cluster) > 0 and i == cluster[-1] + 1:
                cluster.append(i)
                cluster_inds.append(i)
                cluster_groups.append(cluster)
                cluster = []
        
        # Set threshold labels so text doesn't overlap
        points, labels = [], []
        i, group = 0, 0
        while i < fpr.size:
            if i not in cluster_inds:
                # Isolated point
                points.append((fpr[i] + 0.02, tpr[i] + 0.02))
                labels.append("{:.2f}".format(thresholds[i]))
            else:
                # Clustered point
                x, y = 0.0, 0.0
                sub_labels = []
                for i in cluster_groups[group]:
                    x += fpr[i]
                    y += tpr[i]
                    sub_labels.append("{:.2f}".format(thresholds[i]))
                x /= len(cluster_groups[group])
                y /= len(cluster_groups[group])
                label = ", ".join(sub_labels)
                points.append((x + 0.0, y - 0.02))
                labels.append(label)
                group += 1
                    
            i += 1
        
        
        for point, label in zip(points, labels):
            ax.annotate(label, point)
        
        plt.legend(loc="lower right", frameon=False)
        
        fig.savefig("{}_ROC.pdf".format(outfile))
        
        plt.close(fig)


    @property
    def coef_(self):
        return self.learner_.coef_


    @property
    def intercept_(self):
        return self.learner_.intercept_

    
    @property
    def n_iter_(self):
        return self.learner_.n_iter_
        
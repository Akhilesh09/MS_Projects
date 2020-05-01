#!/usr/bin/env python3

# Standard imports
import numpy as np
import graphviz
import matplotlib.pyplot as plt
import os
import time
import warnings
from matplotlib.ticker import MaxNLocator

# Utilities imports
from utilities.correlation_filter import correlation_filter
from utilities.stats import compute_confidence_score

# Scikit-Learn imports
from sklearn.model_selection import StratifiedKFold
from sklearn.model_selection import StratifiedShuffleSplit

# Learner imports
from learners.decision_tree import DecisionTree
from learners.perceptron import Perceptron
from learners.perceptron_forest import PerceptronForest
from learners.knn import KNN
from learners.neural_network import NeuralNetwork
from learners.ensemble_network import EnsembleNetwork
from learners.bayes import NaiveBayes
from learners.bayes_network import BayesNetwork
from learners.bayes_forest import BayesForest

class Project:
    """
    Class for executing CSCE 633 Project routines.
    """

    def __init__(self, data, model, hyperparams={}, data_opts={}, k=3, 
                 confidence=0.95, split_seed=None, verbosity=0):
        """
        Class initilizer.

        Parameters
        ----------
        data : (pandas.DataFrame, pandas.DataFrame)
            Input data for the learning algorithms. Tuple ordered by 
            data and classifications.

        model : str
            Machine learning model to be used:
                'tree': decision tree.
                'perceptron': perceptron.
                'perceptron_forest': perceptron forest.
                'knn': k nearest neighbor.
                'neural_network': neural network.
                'ensemble_network': novel ensemble network.
                
        hyperparams : dict, optional
            Hyperparameters to be used in model:
                See specific learner modules for more documentation
                on possible key : value pairs.
        
        data_opts : dictionary, optional
            Dictionary containing the options for the data set manipulation.
            Default is an empty list meaning that the default parameters will
            be used.
            
            pca_opt : Boolean for PCA use.
            pca_threshold : Fraction of explained variance to keep.
            norm_opt : max / standard / robust / minmax / norm / None
                max = Divide by maximum observed value.
                standard = Remove mean, divide by standard deviation.
                robust = Remove median, divide by inter-quartile range
                minmax = Map to (min, max) range. Defined to be (0, 1)
                None = no data transformation
            domain_opt : global / time.

        k : int > 1, optional
            Number of folds to use in cross-validation. The default is 3.
            
        confidence : float in (0, 1), optional
            Desired level of confidence for accuracy estimation. The default is 0.95.

        split_seed : int, optional
            Random seed for cross-validation splitting. The default is None.
        
        verbosity : int, optional
            Level of verbosity. The default is 0 (no printout).

        Raises
        ------
        ValueError
            Errors for invalid parameter values.

        """

        # Sanity checks
        models = [
            "tree", "perceptron", "perceptron_forest", "knn", "neural_network", "ensemble_network",
            "bayes", "bayes_forest", "bayes_network" 
        ]
        if model not in models:
            raise ValueError(
                "Unrecognized model '{}'. Valid models are {}.".format(
                    model, ", ".join(models)
                )
            )

        if not isinstance(k, int) or k < 1:
            raise ValueError("k must be an integer greater than zero.")

        if not (0.0 < confidence < 1.0):
            raise ValueError("Confidence must be between 0 and 1.")
        
        self.model = model

        # Get data
        self.X = data[0]
        self.y = data[1]
        self.features = np.array(data[0].columns)
        self.programs = np.array(data[0].index)

        # K for K-Fold
        self.k = k

        # Confidence interval
        self.confidence = confidence

        # Random seed
        self.split_seed = split_seed

        # Hyperparams
        self.hyperparams=hyperparams
        
        # Initialize learning model
        if model == "tree":

            self.learner = DecisionTree(**hyperparams, data_opts=data_opts)
            
        elif model == "perceptron":
        
            self.learner = Perceptron(**hyperparams, data_opts=data_opts)

        elif model == "perceptron_forest":

            self.learner = PerceptronForest(**hyperparams, data_opts=data_opts)

        elif model == "knn":
            
            self.learner = KNN(**hyperparams, data_opts=data_opts)

        elif model == "neural_network":

            self.learner = NeuralNetwork(**hyperparams, data_opts=data_opts)         

        elif model == "ensemble_network":

            self.learner = EnsembleNetwork(**hyperparams, data_opts=data_opts)   

        elif model == "bayes":

            self.learner = NaiveBayes(**hyperparams, data_opts=data_opts)  

        elif model == "bayes_forest":
            
            self.learner = BayesForest(**hyperparams, data_opts=data_opts)

        elif model == "bayes_network":

            self.learner = BayesNetwork(**hyperparams, data_opts=data_opts)
        
        self.verbosity = verbosity

    def evaluate_model_accuracy(self, outdir=None, test_size=0.33):

        """
        Performs K-fold/single-fold cross validation on a model to 
        estimate its accuracy.
        
        Parameters
        ----------
        outdir : str
            Directory to which to write output files.
        test_size : float, optional
            Used if single fold accuracy is evaluated. Puts test_size of the data
            aside for validation. Uses (1-test_size) to train.
            Default value is 0.33.

        Returns
        -------
        Tuple of two tuples. The first contains information about model
        accuracy such as best fold accuracy, average fold accuracy, the
        standard deviation, and the z/t score. The second contains the 
        training and testing times.
        """
        
        # Prep output directory
        if outdir != None:
            if not os.path.isdir(outdir):
                os.makedirs(outdir)
        
        # Confusion Matrix values
        self.false_positives=[]
        self.false_negatives=[]
        self.true_positives=[]
        self.true_negatives=[]
        self.total_positives=[]
        self.total_negatives=[]

        # Evaluate model
        if self.k > 1:
            return self.evaluate_k_fold_accuracy(outdir)
        else:
            return self.evaluate_single_fold_accuracy(outdir, test_size)
        
    def evaluate_k_fold_accuracy(self, outdir=None):
        """
        Performs K-fold cross validation on a model to estimate its accuracy.
        
        Parameters
        ----------
        outdir : str
            Directory to which to write output files.

        Returns
        -------
        Tuple of two tuples. The first contains information about model
        accuracy such as best fold accuracy, average fold accuracy, the
        standard deviation, and the z/t score. The second contains the 
        training and testing times.
        """

        if outdir is not None:
            # Make sure directory exists
            odir = outdir[:outdir.rfind("/")]
            if "/" in outdir and not os.path.isdir(odir):
                os.makedirs(odir)

        # Initialize K-Fold splittling
        kf = StratifiedKFold(n_splits=self.k, shuffle=True, random_state=self.split_seed)

        # Convert X and y to numpy arrays
        X, y = self.reformat_data(self.X, self.y)

        # Train, test, and compute learner errors
        fold_accuracies, fold_variances = [], []
        train_time, test_time = 0., 0. # total train/test time
        ik = 0
        weights = []
        for itrain, itest in kf.split(self.X, y):
            # Perform transformations on training data
            X_train = self.X.iloc[itrain]
            y_train = y[itrain]
            
            # Fit model to train data
            tic = time.perf_counter_ns() # Start timer
            self.learner.fit(X_train, y_train)
            toc = time.perf_counter_ns() # Stop timer
            train_time += toc-tic # add time

            # Apply transformations on test data
            X_test = self.X.iloc[itest]
            y_test = y[itest]

            # Score the model on test data
            acc, t = self.gen_confusion_matrix_values(X_test, y_test)
            test_time += t # add time

            # Compute variance of fold
            var = acc * (1 - acc) / len(y_test)
            weights.append(len(y_test))

            fold_accuracies.append(acc)
            fold_variances.append(var)
            
            # Write information about trained model to files
            CV_learner = self.learner.get_CV_info(X_test, y_test)
            if outdir is not None:
                self.learner.write_CV_learner(CV_learner, ik, self.features, outdir)
            
            # Keep track of best model
            if acc >= max(fold_accuracies):
                self.best_CV = self.learner.get_CV_info(X_test, y_test)

            ik += 1

        # Outputs for tree and perceptron model
        # save info about the best performing CV case to file
        if outdir is not None:
            self.learner.write_CV_learner(self.best_CV, "best", self.features, outdir)

        mean_accuracy = np.average(fold_accuracies, weights=weights)
        std = np.sqrt(np.sum(fold_variances)/len(fold_variances)**2)
        train_time *= 1E-9
        test_time *= 1E-9
        
        score = compute_confidence_score(acc, len(self.y), self.confidence, self.k)
        
        if self.verbosity > 0:
            # Print results
            self.print_stats(mean_accuracy, std, score, fold_accuracies, train_time, test_time)

        self.best_fold_index=fold_accuracies.index(max(fold_accuracies))
        return ((max(fold_accuracies), mean_accuracy, std, score), (train_time, test_time))

    def evaluate_single_fold_accuracy(self, outdir=None, test_size=0.33):
        """
        Evaluate the model accuracy using one training and test set 
        to estimate its accuracy.
        
        Parameters
        ----------
        outdir : str
            Directory to which to write output files.
        test_size : float, optional
            Specified the fraction of data to be set aside as test data. The larger,
            the better the statistics. The defualt value is 0.6.

        Returns
        -------
        The return signature is made to match that of evaluate_k_fold_accuracy,
        so some return values are duplicated. See the returns for 
        evaluate_k_fold_accuracy for details.
        """
        
        # Default value check
        # if len(self.evaluate_single_fold_accuracy.__defaults__) != 1:
        #     raise ValueError("This function should always have a parameter "
        #                      "'test_size' with a default value. This should "
        #                      "be the only parameter with a defualt value.")
        # else:
        #     if test_size != self.evaluate_single_fold_accuracy.__defaults__[0]:
        #         raise ValueError("Parameter 'test_size' must always be equal to "
        #                          "its default value. If another value is desired, "
        #                          "change the default.")

        # Reformat data for learners
        X, y = self.reformat_data(self.X, self.y)

        cv = StratifiedShuffleSplit(
            test_size=test_size, random_state=self.split_seed
        )
        itrain, itest = next(cv.split(X, y))

        # Perform transformations on training data
        X_train = self.X.iloc[itrain]
        y_train = y[itrain]

        # Fit the learner
        tic = time.perf_counter_ns() # start timer
        self.learner.fit(X_train, y_train)
        toc = time.perf_counter_ns() # end timer
        train_time = (toc-tic)*1E-9

        # Perform transformations on test data
        X_test = self.X.iloc[itest]
        y_test = y[itest]
        
        # Score the learner
        acc, t = self.gen_confusion_matrix_values(X_test, y_test)
        test_time = t*1E-9

        # Standard deviation / variacnce
        var = acc * (1 - acc) / len(y_test)
        std = np.sqrt(var)
        
        score = compute_confidence_score(acc, len(y_test), self.confidence, self.k)
        
        if self.verbosity > 0:
            # Print results
            self.print_stats(acc, std, score, [acc], train_time, test_time)

        return ((acc, acc, np.sqrt(var), score), (train_time, test_time))

    def gen_confusion_matrix_values(self, X_test, y_test):
        """
        Generate confusion matrix given test set (X, y). Returns accuracy
        and classification time.
        """
        # Generate predictions
        tic = time.perf_counter_ns() # Start timer
        predictions = self.learner.predict(X_test)
        toc = time.perf_counter_ns() # Stop timer

        # Build up confusion matrix values
        
        ind_pos = np.argwhere(y_test == 1)
        ind_neg = np.argwhere(y_test == -1)
        
        total_pos, total_neg = len(ind_pos), len(ind_neg)
        
        tp = np.sum(predictions[ind_pos] ==  1)
        fn = np.sum(predictions[ind_pos] == -1)
        
        tn = np.sum(predictions[ind_neg] == -1)
        fp = np.sum(predictions[ind_neg] ==  1)

        self.total_positives.append(total_pos)
        self.total_negatives.append(total_neg)
        self.true_positives.append(tp)
        self.true_negatives.append(tn)
        self.false_positives.append(fp)
        self.false_negatives.append(fn)

        return (tp+tn)/(total_pos+total_neg), toc - tic
    
    def print_stats(self, mean_accuracy, std, score, fold_accuracies, train_time, test_time):
        """
        Function for print statistical information such as accuracy, standard
        deviation, and confidence interval given accuracy and time data.
        """
        separator = "=" * 45
        print("{} model\n{}".format(self.model, separator))

        print("{} & {} & {} & {} & {} & {} \\\\ \hline \hline".format(
                "\\textbf{Fold}", "\\textbf{TP}", "\\textbf{TN}", 
                "\\textbf{FP}", "\\textbf{FN}", "\\textbf{Accuracy}"
            )
        )
        for i in range(self.k):
            acc = (
                (self.true_positives[i] + self.true_negatives[i])
                / (self.total_positives[i] + self.total_negatives[i])
            )
            msg = "{:d} & {:d} & {:d} & {:d} & {:d} & {:.4f}  \\\\ \hline"
            print(msg.format(
                    i+1, self.true_positives[i], self.true_negatives[i],
                    self.false_positives[i], self.false_negatives[i], acc
                )
            )
        print("{}".format(separator))
        print("{:<20} & {:<20} \\\\ \hline \hline".format("\\textbf{Quantity}", "\\textbf{Value}"))
        print("{:<20} & {:.4f} \\\\ \hline".format("Mean Accuracy", mean_accuracy))
        print("{:<20} & {:.4f} \\\\ \hline".format("Standard Deviation", std))
        if score is not None:
            print(
                "{:<20} & {:.4f} $\pm$ {:.4f} \\\\ \hline".format(
                    "{:.2f}\% Confidence".format(100*self.confidence), 
                        mean_accuracy, score * std
                )
            )
            print(
                "{:<20} & ({:.4f}, {:.4f}) \\\\ \hline".format(
                    "Accuracy Range",
                    max(0, mean_accuracy - score * std),
                    min(mean_accuracy + score * std, 1),
                )
            )

        print("{:<20} & {:.4f} \\\\ \hline".format("Best Accuracy", max(fold_accuracies)))
        print("{:<20} & {:.4f} s\\\\ \hline".format("Training Time", train_time))
        print("{:<20} & {:.4f} s \\\\ \hline".format("Classification Time", test_time))

        print("{}\n".format(separator))
    
    @staticmethod
    def reformat_data(X, y):
        return X.values, y.values.ravel()

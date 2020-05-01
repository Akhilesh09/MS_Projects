#!/usr/bin/env python3

import graphviz
from copy import deepcopy
from sklearn import tree
from sklearn.tree import DecisionTreeClassifier
from learners.learner_base import LearnerBase
import inspect

class DecisionTree(LearnerBase):
    """
    Wrapper function for scikit-learn decision tree.
    """

    def __init__(self, max_depth=None, criterion='entropy', ccp_alpha=0.0,
                 random_state=3, splitter='best', min_samples_split=2,
                 min_samples_leaf=1, min_weight_fraction_leaf=0.0,
                 max_features=None, max_leaf_nodes=None, min_impurity_decrease=0.0,
                 min_impurity_split=None, class_weight=None, data_opts={}):
        """
        Class initilizer

        Parameters
        ----------
        max_depth = maxumum allowable depth of the tree (default: None)
        criterion = criterion used to determine attribute to split on
                    ('entropy' or 'gini', default: 'entropy')
        ccp_alpha = parameter controlling pruning (default: 0.0, no pruning)

        random_state : int, optional
            Seed for random shuffling. The default is 3.
        
        data_opts : dictionary
            Dictionary containing the options for the data set manipulation.
            Default is an empty dictionary meaning that the default parameters
            will be used. See project class for more information on valid inputs.
            
        others: see parameters to sklearn's sklearn.treeDecisionTreeClassifier.
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
        self.learner_ = DecisionTreeClassifier(**params)

    def get_CV_info(self, X, y):
        return deepcopy(self.learner_)


    def write_CV_learner(self, CV_learner, ID, labels, outdir):

        # Generate outfile path
        outfile = "{}/DT_{}".format(outdir, ID)

        # Write DT to files
        self.write_DT(CV_learner, outfile, labels)
        
    @staticmethod
    def write_DT(DT, outfile, labels, generate_sorted_file=True):
        """
        Graphs the DT, writes the DT as text, and generates 'DT_sorted.txt' file
        that ranks features in descending order.

        Parameters
        ----------
        DT : sklearn.tree.DecisionTreeClassifier
            Decision tree of interest.
        outfile : str
            path to files to be generated.
        generate_sorted_file : bool, optional
            Whether to generate a file ranking the tree's features.
            The default is True.
        """

        # Make graph
        dot_data = tree.export_graphviz(
            DT,
            out_file=None,
            feature_names=labels,
            class_names=["benign", "malicous"],
            filled=True,
            rounded=True,
            special_characters=True,
            precision=6
        )
        graph = graphviz.Source(dot_data)
        graph.render("{}.gv".format(outfile), view=False)
        
        # Write DT as text
        with open("{}.txt".format(outfile), "w") as text_file:
            text_file.write(
                tree.export_text(
                    DT,
                    feature_names=list(labels),
                    show_weights=True,
                )
            )
        
        features = []

        if generate_sorted_file:
            # write DT_sorted.txt
            with open("{}_sorted.txt".format(outfile), "w") as DT_sorted:
                # Read text tree for best k
                with open("{}.txt".format(outfile), "r") as text_tree:
                    for line in text_tree:
                        if "class: " in line:
                            continue
                        else:
                            # string containing attribute test
                            attribute_test = line.strip().split("|--- ")[-1]
                            # extract feature from attibute test
                            feature = attribute_test.split("<")[0].split(">")[0].strip()
    
                            if feature not in features:
                                features.append(feature)
                                DT_sorted.write(feature + "\n")

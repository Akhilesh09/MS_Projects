#!/usr/bin/env python3

"""
This program runs the studies on the Bayesian Network for data prepared using different normalization techniques.
"""

import os
import numpy as np
import pandas as pd
from utilities.data_parser import DataParser
from utilities.functions import get_classifications 
from utilities.correlation_filter import correlation_filter
from utilities.searches import grid_search
from project import Project
from studies.bayes_network_studies import get_bayes_network_studies
from studies.bayes_network_studies import bayes_network_grid_search
from utilities.searches import transformation_search


# Paths
case = "941_486"
data_path = os.path.join("..", "parsed_data", "allstats_941_486.txt")
programs_path = os.path.join("..", "parsed_data", "programsnames_941.txt")
# Filtering option
corr_threshold = 0.99

# Prepare data
parser = DataParser()
X = parser.parse_datafile(data_path, programs_path)
y = get_classifications(programs_path)
data = (X, y)
# Filter data
data = correlation_filter(data, threshold=corr_threshold)


# Study 1- Normalization/transformation options
opts = {
    "pca_opt" : False,
    "pca_threshold" : 0.99, 
    "norm_opt" : "standard",
    "domain_opt" : "global"
}

get_bayes_network_studies(opts,data_path, programs_path,corr_threshold)

# Study 2- Normalization/transformation options
opts = {
    "pca_opt" : False,
    "pca_threshold" : 0.99, 
    "norm_opt" : "standard",
    "domain_opt" : "time"
}

# get_ensemble_network_studies(opts,data_path, programs_path,corr_threshold)

# transformation_search(data, "neural_network", 3)

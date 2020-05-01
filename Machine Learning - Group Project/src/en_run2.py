#!/usr/bin/env python3

"""
This program runs the studies on the Ensemble Network for data prepared using different normalization techniques.
"""

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from utilities.data_parser import DataParser
from utilities.functions import get_classifications 
from utilities.correlation_filter import correlation_filter
from utilities.searches import grid_search
from project import Project
from studies.ensemble_network_studies import ensemble_network_architecture_study


# =====> Get the data
data_dir = os.path.realpath('../parsed_data')
out_dir = os.path.realpath('../output/EN/figures/')

normalizations = [["global", "max"]]
filter_values = [1.0, 0.9]
filter_type="feature"
corr_type="pearson"

case = "_941_486"

for normalization in normalizations:
    for filter_value in filter_values:

        # Creating tags for output file names
        tag = ""
        if filter_value < 1.0:
            tag = "_"+filter_type+"_"+corr_type+"_"+"{:.0%}".format(filter_value)[:-1]

        print("--------------------------------------------------------------")
        print("allstats_"+"_".join(normalization)+case+tag)
        print("--------------------------------------------------------------")
        
        fname = "allstats"+case+".txt"
        data_path = os.path.join("..", "parsed_data", fname)
        programs_path = os.path.join("..", "parsed_data", "programsnames_941.txt")

        opts = {
            "pca_opt" : False,
            "pca_threshold" : 0.99, 
            "norm_opt" : normalization[1],
            "domain_opt" : normalization[0]
        }

        # Prepare data
        parser = DataParser()
        X = parser.parse_datafile(data_path, programs_path)
        y = get_classifications(programs_path)
        data = (X, y)

        # Filtering data based on correlation
        data = correlation_filter(data, threshold=filter_value, verbosity=2)

        # Random seed 
        seed = 3

        # Studies
        fname = os.path.join(out_dir, "en_architecture_"+"_".join(normalization)+case+tag+".pdf")
        ensemble_network_architecture_study(
            data, opts=opts, num_seeds=10, kcv=3, filename=fname)
        plt.show()

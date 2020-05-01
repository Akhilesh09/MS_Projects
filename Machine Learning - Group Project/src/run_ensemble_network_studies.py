#!/usr/bin/env python3

"""
This program runs the studies on the Ensemble Network fo data prepared using different normalization techniques.
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
from studies.ensemble_network_studies import ensemble_network_grid_search
from studies.ensemble_network_studies import ensemble_network_architecture_study
from studies.ensemble_network_studies import analyze_best_model


# =====> Get the data
data_dir = os.path.realpath('../parsed_data')
out_dir = os.path.realpath('../output/EN/figures/')

normalizations = [["global", "standard"],["global", "max"],["time", "standard"], ["time", "max"]]
filter_values = [1.0, 0.9]
filter_type="feature"
corr_type="pearson"

case = "_941_486"


best_models, best_params, accs = [], [], np.array([])
for normalization in normalizations:
    for filter_value in filter_values:

        # Creating tags for output file names
        tag = ""
        if filter_value < 1.0:
            tag = "_"+filter_type+"_"+corr_type+"_"+"{:.0%}".format(filter_value)[:-1]

        
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
        data = correlation_filter(data, threshold=filter_value, verbosity=0)

        # Random seed 
        seed = 3

        fname = os.path.join(
            out_dir, 
            "en_architecture_"+"_".join(normalization)+case+tag+".fig"
        )

        best_model, best_param = analyze_best_model(fname, data, seed, opts)
        best_param["domain"] = normalization[0]
        best_param["normalization"] = normalization[1]
        best_param["filter_value"] = filter_value

        msg = "Most Accurate Model: {}".format(best_param)
        msg = "\n".join(["-"*len(msg), msg, "-"*len(msg),""])
        print(msg)
        
        # Evaluate best model
        outp = best_model.evaluate_model_accuracy()
        best_models.append(best_model)
        best_params.append(best_param)
        accs = np.append(accs, outp[0][1])
    
ind = np.argsort(-1*accs)
accs = accs[ind]
best_models = np.asarray(best_models)[ind]
best_params = np.asarray(best_params)[ind]
print()
print(accs)
print()

msg = "Top 3 Models:"
msg = "\n".join(["\n", "="*len(msg), msg, "="*len(msg)])
print(msg)
for i in range(3):
    msg = "Params: {}".format(best_params[i])
    msg = "\n".join([msg, "-"*len(msg), ""])
    print(msg)
    best_models[i].evaluate_model_accuracy()

    

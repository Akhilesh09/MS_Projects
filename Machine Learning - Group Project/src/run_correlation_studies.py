#!/usr/bin/env python3

import os
import numpy as np
import pandas as pd
from utilities.data_parser import DataParser
from utilities.functions import get_classifications
from studies.correlation_studies import feature_feature_correlation_study
from studies.correlation_studies import feature_label_correlation_study
from utilities.correlation_filter import create_filtered_data_sets

# Setting verbosity level (to see everything)
verbosity = 2

# =====> Get the data paths
data_dir = os.path.realpath('../parsed_data')
outdir_figures = os.path.realpath('../output/features/figures')
outdir_text = os.path.realpath('../output/features')

programs_path = os.path.join("..", "parsed_data", "programsnames_941.txt")

corr_type = "pearson"
filter_type = "feature"

# Normalization/transformation options

tags = [["global","max"], ["global","gauss"], ["time","max"], ["time","gauss"]]

# Creating new, filtered data bases
for i in range(len(tags)):

	tag = tags[i][0]+"_"+tags[i][1]
	fname = "allstats_"+tag+"_941_486.txt"
	data_path = os.path.join("..", "parsed_data", fname)

	# Prepare data
	parser = DataParser()
	X = parser.parse_datafile(data_path, programs_path)
	y = get_classifications(programs_path)
	data = (X, y)

	print("******************************************************************")
	print("CREATING FILTERED DATABASES")
	create_filtered_data_sets(data, fname, data_dir, 
	                      	  filter_type=filter_type, corr_type=corr_type, 
                          	  thresholds=[0.99, 0.95, 0.9, 0.8, 0.7],
                          	  verbosity=verbosity)

# Creating figures for correlation studies
for i in range(len(tags)):

	print("******************************************************************")
	print("CREATING FEATURE-FEATURE PLOTS")
	# Creating plot for feature-feature correlations
	file_name = os.path.join(outdir_figures, tag+"_feature_correlation.pdf")
	feature_feature_correlation_study(data, filename=file_name)

	print("******************************************************************")
	print("CHECKING FEATURE-LABEL CORRELATIONS")
	# Creating sorted lists for feature-label correlations
	feature_label_correlation_study(data, outdir=outdir_text, tag=tag)
	print("******************************************************************")


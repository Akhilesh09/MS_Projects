#!/usr/bin/env python3

import os
import numpy as np
import pandas as pd
from utilities.data_parser import DataParser
from utilities.correlation_filter import correlation_filter
from utilities.functions import get_classifications
from learners.neural_network import NeuralNetwork

from studies.neural_network_studies import neural_network_architecture_study
from studies.neural_network_studies import neural_network_convergence_study
from studies.neural_network_studies import modify_neural_network_figure
from studies.neural_network_studies import analyze_best_model


# =====> Get the data
data_dir = os.path.realpath('../parsed_data')
out_dir = os.path.realpath('../output/neural_network/figures/')

normalizations = [["global", "standard"],["global", "max"],["time", "standard"], ["time", "max"]]
activations = ["relu", "logistic", "tanh"]
filter_values = [1.0, 0.9]
filter_type="feature"
corr_type="pearson"

case = "_941_486"

# Select what experiment is needed to be run
# 1 = Creating 3D plots of the architecture dependent accuracy
# 2 = Creating epoch = f(batch size) plots
# 3 = Analyzing best models from already created data
# 4 = Both 1 and 2
select_run = 1

# Selecting the appropriate model
if select_run in [2,4]:

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

			# Prepare the batch sizes. The resolution is finer in the lower end to
			# make sure we resolve the high train time gradient 
			no_samples = int(0.66*len(data[0].index))
			no_bathces = 12
			batch_sizes_start = np.linspace(1, 5, 5, 
			                            endpoint=True, dtype=int)
			batch_sizes_end = np.linspace(6, no_samples, 7, 
			                          endpoint=True, dtype=int)

			batch_sizes = np.concatenate((batch_sizes_start, batch_sizes_end))

			# Random seed 
			seed = 3

			# Studies
			fname = os.path.join(out_dir, "nn_convergence_"+"_".join(normalization)+case+tag+".pdf")
			neural_network_convergence_study(data, opts=opts, num_seeds=10, batch_sizes=batch_sizes, 
			                             activations=activations, filename=fname)
if select_run in [1,4]:

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
			
			fname = os.path.join(out_dir, "nn_architecture_"+"_".join(normalization)+case+tag+".pdf")
			neural_network_architecture_study(data, opts=opts, num_seeds=10, max_layers=5, 
				                              max_neuron_per_layer=10, kcv=3, filename=fname)
			modify_neural_network_figure(fname.replace(".pdf",".fig"), filename_new=fname)
	
			analyze_best_model(fname.replace(".pdf",".fig"), data, seed, opts=opts, kcv=3)

if select_run in [3]:

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
			
			fname = os.path.join(out_dir, "nn_architecture_"+"_".join(normalization)+case+tag+".pdf")	
			analyze_best_model(fname.replace(".pdf",".fig"), data, seed, opts=opts, kcv=3)

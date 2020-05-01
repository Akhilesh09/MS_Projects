#!/usr/bin/env python3

"""
This program gives simple examples for running each learner.
"""

import os
from utilities.data_parser import DataParser
from utilities.functions import get_classifications
from utilities.stats import run_mcnemar_tests  
from utilities.correlation_filter import correlation_filter
from project import Project

### Problem definition ###
# Problem and paths
case = "941_486"
data_path = os.path.join("..", "parsed_data", "allstats_941_486.txt")
programs_path = os.path.join("..", "parsed_data", "programsnames_941.txt")
# Filtering option
corr_threshold = 1.0
# Normalization/transformation options
opts = {
    "pca_opt" : False, # True / False
    "pca_threshold" : 1.0, # (0,1)
    "norm_opt" : "standard", # max / standard / robust / minmax / norm / None
    "domain_opt" : "global" # global / time
}

opt_ID = case
tmp = str(opts["pca_threshold"])
tail = ("pca_{}".format(tmp[tmp.find('.')+1:]) if opts["pca_opt"]
        else "{}_{}".format(opts["domain_opt"],opts["norm_opt"]))
opt_ID += "_" + tail

# Run options
grid_search = False
# Output options
output = False
outbase = os.path.join("..", "output")

### Data Preparation ###
msg = ">"*10 + " "
msg += "Preprocessing phase..."
print(msg)

# Prepare data
parser = DataParser()
X = parser.parse_datafile(data_path, programs_path)
y = get_classifications(programs_path)
# Filter data
X, y = correlation_filter((X, y), threshold=corr_threshold)
data = (X, y)

learners=[]

### Run learning models
 
msg = msg = ">"*10 + " "
msg += "Learner evaluation phase..."
print(msg)

# Parameters
split_seed = 3 # Random seed 
verbosity = 2

# === Decision tree
###############################################################################
if output:
    outdir = os.path.join(outbase, "DT", "DT_"+case)
    outfile = os.path.join(outdir, "_".join(["DT", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.DT_studies import DT_grid_search
    best_params = DT_grid_search(data, data_opts=opts, k=10, n_repeats=10, filename=outfile, seed=split_seed)
else:
    best_params = {"max_depth" : None, "ccp_alpha" : 0.0}

tree = Project(data, "tree", best_params, opts, split_seed=split_seed, verbosity=verbosity)
tree.evaluate_model_accuracy(outdir=outdir)
if verbosity > 0:
    print('\n')

learners.append(tree)

### === Perceptron
###############################################################################
if output:
    outdir = os.path.join(outbase, "Perceptron", "P_"+case)
    outfile = os.path.join(outdir, "_".join(["P", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.perceptron_studies import perceptron_grid_search
    best_params = perceptron_grid_search(data, data_opts=opts, k=10, n_repeats=10, filename=outfile, seed=split_seed)
else:
    best_params = {"penalty" : None, "alpha" : 0.0}

perceptron = Project(data, "perceptron", best_params, opts, split_seed=split_seed, verbosity=verbosity)
perceptron.evaluate_model_accuracy(outdir=outdir)
if verbosity > 0:
    print('\n')
    
learners.append(perceptron)

### === Perceptron Forest
###############################################################################

if output:
    outdir = os.path.join(outbase, "PF", "PF_"+case)
    outfile = os.path.join(outdir, "_".join(["PF", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.PF_studies import PF_grid_search
    best_params = PF_grid_search(data, data_opts=opts, k=2, n_repeats=1, filename=outfile, seed=split_seed)
else:
    best_params = {"max_samples" : 0.4, "n_estimators" : 2}

forest = Project(data, "perceptron_forest", best_params, opts, split_seed=split_seed, verbosity=verbosity)
forest.evaluate_model_accuracy(outdir=outdir)
if verbosity > 0:
    print('\n')
  
learners.append(forest)

### === KNN
###############################################################################
if output:
    outdir = os.path.join(outbase, "KNN", "KNN_"+case)
    outfile = os.path.join(outdir, "_".join(["KNN", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.knn_studies import KNN_grid_search
    best_params = KNN_grid_search(data, data_opts=opts, k=10, n_repeats=100, filename=outfile, seed=split_seed)
else:
    best_params = {"weights" : "dist_sq", "n_neighbors" : 3}

knn = Project(data, "knn", best_params, opts, split_seed=split_seed, verbosity=verbosity)
knn.evaluate_model_accuracy()
if verbosity > 0:
    print('\n')
    
learners.append(knn)

### === Neural Network
###############################################################################
if output:
    outdir = os.path.join(outbase, "NN", "NN_"+case)
    outfile = os.path.join(outdir, "_".join(["NN", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.neural_network_studies import neural_net_grid_search
    best_params =neural_net_grid_search(data, data_opts=opts, k=2, n_repeats=1, filename=outfile, seed=split_seed)
else:
    best_params = {"hidden_layer_sizes" : (9,9)}
    
nn = Project(data, "neural_network", best_params, opts, split_seed=split_seed, verbosity=verbosity)
nn.evaluate_model_accuracy(outdir=outdir)
if verbosity > 0:
    print('\n')

learners.append(nn)

# === Ensemble Network
###############################################################################
if output:
    outdir = os.path.join(outbase, "EN", "EN_"+case)
    outfile = os.path.join(outdir, "_".join(["EN", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.ensemble_network_studies import ensemble_network_grid_search
    best_params = ensemble_network_grid_search(data, data_opts=opts, k=10, n_repeats=10, filename=outfile, seed=split_seed)
else:
    best_params = {"hidden_layer_sizes" : (9,9), "n_estimators" : 8}

en = Project(data, "ensemble_network", best_params, opts, split_seed=split_seed, verbosity=verbosity)
en.evaluate_model_accuracy(outdir=outdir)
if verbosity > 0:
    print('\n')

learners.append(en)

# run McNemar's Test for unique pairs of learners
while(len(learners)>0):
    for model in learners[1:]:
        run_mcnemar_tests(learners[0],model,X,y.values)
    del learners[0]

"""
# === Bayes Network
###############################################################################
if output:
    outdir = os.path.join(outbase, "BN", "BN"+case)
    outfile = os.path.join(outdir, "_".join(["BN", "gridsearch", opt_ID])) + ".pdf"
else:
    outdir = None
    outfile = None
if grid_search:
    from studies.bayes_network_studies import bayes_network_grid_search
    best_params = bayes_network_grid_search(data, data_opts=opts, k=10, n_repeats=10, filename=outfile, seed=split_seed)
else:
    best_params = {}

bn = Project(data, "bayes_network", best_params, opts, split_seed=split_seed, verbosity=verbosity)
bn.evaluate_model_accuracy(outdir=outdir)
"""
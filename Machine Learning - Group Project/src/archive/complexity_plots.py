import os
from prettytable import PrettyTable
from utilities.functions import get_data
from project import Project
import numpy as np

tb_training_time = PrettyTable()

tb_training_time.field_names = ["Decision Tree", "Perceptron", "Perceptron Tree", "KNN brute", "KNN ball tree"]

tb_testing_time = PrettyTable()

tb_testing_time.field_names = ["Decision Tree", "Perceptron", "Perceptron Tree", "KNN brute", "KNN ball tree"]


# =====> Get the data
data_dir = os.path.realpath('../parsed_data')

case = "_941_491"

# Parse the raw data
X_DF, y_DF = get_data(data_dir, data_file="allstats_norm" + case + ".txt")
labels = list(X_DF.columns)

# =====> Run learning models
# Define directory to output data
outdir = os.path.realpath("../output")

# Random seed
seed = 3

# === Decision tree
params = (None, "entropy", 0.05)
tree = Project((X_DF, y_DF), "tree", params, seed=seed, verbosity=1)
tree.time_samples_trend(labels, os.path.join(outdir, "DT" + case))
tree.time_features_trend(labels, os.path.join(outdir, "DT" + case))

# === Perceptron
params = (0.1, 1000, 1e-3)
perceptron = Project((X_DF, y_DF), "perceptron", params, seed=seed, verbosity=1)
perceptron.time_samples_trend(labels, os.path.join(outdir, "P" + case))
perceptron.time_features_trend(labels, os.path.join(outdir, "P" + case))

# === Perceptron Forest
params = ((), (), (15, 0.5, True)) # 10 weak learners (DTs) that each use 50% of the data with replcement
forest = Project((X_DF, y_DF), "perceptron_forest", params, seed=seed, verbosity=1)
forest.time_samples_trend(labels, os.path.join(outdir, "PF" + case))
forest.time_features_trend(labels, os.path.join(outdir, "PF" + case))

# === KNN, brute
params = (3, "distance", "brute")
knn = Project((X_DF, y_DF), "knn", params, seed=seed, verbosity=1)
# knn.time_samples_trend_knn(labels, os.path.join(outdir, "KNN", "best_k"+case+".pdf"))
knn.time_features_trend_knn(labels, os.path.join(outdir, "KNN", "best_k"+case+".pdf"))





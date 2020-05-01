import os
from prettytable import PrettyTable
from utilities.functions import get_data
from project import Project

tb_training_time = PrettyTable()

tb_training_time.field_names = ["Decision Tree", "Perceptron", "Perceptron Tree"]

tb_testing_time = PrettyTable()

tb_testing_time.field_names = ["Decision Tree", "Perceptron", "Perceptron Tree", "KNN brute"]


# =====> Get the data
data_dir = os.path.realpath('../parsed_data')

case = ["_39_63", "_39_491", "_941_63", "_941_491"]

for dataset in case:

    # Parse the raw data
    X_DF, y_DF = get_data(data_dir, data_file="allstats_norm" + dataset + ".txt")
    labels = list(X_DF.columns)

    # =====> Run learning models
    # Define directory to output data
    outdir = os.path.realpath("../output")

    # Random seed
    seed = 3

    # === Decision tree
    params = (None, "entropy", 0.05)
    tree = Project((X_DF, y_DF), "tree", params, seed=seed, verbosity=1)
    DT_training_time, DT_testing_time = tree.time_complexity(labels, os.path.join(outdir, "DT" + dataset))
    print(DT_training_time, DT_testing_time)

    # === Perceptron
    params = (0.1, 1000, 1e-3)
    perceptron = Project((X_DF, y_DF), "perceptron", params, seed=seed, verbosity=1)
    PPN_training_time, PPN_testing_time = perceptron.time_complexity(labels, os.path.join(outdir, "P" + dataset))
    print(PPN_training_time, PPN_testing_time)

    # === Perceptron Forest
    params = ((), (), (15, 0.5, True))  # 10 weak learners (DTs) that each use 50% of the data with replcement
    forest = Project((X_DF, y_DF), "perceptron_forest", params, seed=seed, verbosity=1)
    PF_training_time, PF_testing_time = forest.time_complexity(labels, os.path.join(outdir, "PF" + dataset))
    print(PF_training_time, PF_testing_time)

    # === KNN, brute
    params = (3, "distance", "brute")
    knn = Project((X_DF, y_DF), "knn", params, seed=seed, verbosity=1)
    KNN_testing_time = knn.time_complexity(labels, os.path.join(outdir, "KNN", "best_k"+dataset+".pdf"))[1]

    # add rows
    tb_training_time.add_row([DT_training_time, PPN_training_time, PF_training_time])
    tb_testing_time.add_row([DT_testing_time, PPN_testing_time, PF_testing_time,
                            KNN_testing_time])


# add a column
tb_training_time.add_column("Sample Number", ["39 samples", "39 samples", "941 samples", "941 samples"])
tb_training_time.add_column("Feature Number", ["63 features", "491 features", "63 features", "491 features"])
tb_testing_time.add_column("Sample Number", ["39 samples", "39 samples", "941 samples", "941 samples"])
tb_testing_time.add_column("Feature Number", ["63 features", "491 features", "63 features", "491 features"])

# add a title
tb_training_time.get_string(title="Training Time(unit: s)")
tb_testing_time.get_string(title="Testing Time(unit: s)")

print("table1. Training Time(unit: s)")
print(tb_training_time)
print("table2. Testing Time(unit: s)")
print(tb_testing_time)


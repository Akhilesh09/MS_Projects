# -*- coding: utf-8 -*-
"""
Experiment file for correlation part. Produces plots seen in report.
"""
import os
import numpy as np
import matplotlib.pyplot as plt

from utilities.functions import get_data
from experiments.feature_ranking_experiments import FeatureSorter, score_compare_plot, box_plot_for_classif, get_best_fts_perceptron
from matplotlib import pyplot as plt

programs_path = os.path.realpath("../parsed_data")

scr_func = 0 # 0 for chi2 test, 1 for F test
# choose the top features based on this score, switch between 0 and 1 to get all report plots
base_sc = 0

# box plot

if scr_func == 0:
    tail = "chi2"
else:
    tail = "f_classif"

# change this between "_39_491" and "_941_491" to get plots in report    
case = "_39_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

stf = FeatureSorter(score_function=scr_func, percent=10)
stf.fit(dataset, malicious) # train the object
sort_ft = stf.sorted_fts

plot_file = os.path.join("../output/features/figures", "box_"+tail+case+".pdf")
box_plot_for_classif(dataset, malicious, sorted_features=sort_ft, ft_num=3, file_path= plot_file)

# effect of different numbers of samples

stf1 = FeatureSorter(score_function=0, percent=10)
stf1.fit(dataset, malicious) # train the object

case = "_941_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

stf2 = FeatureSorter(score_function=0, percent=10)
stf2.fit(dataset, malicious) # train the object

scores1 = stf1.scores
scores2 = stf2.scores
fts1 = stf1.X.columns
fts2 = stf2.X.columns

scores = np.array([scores1,scores2])
features = np.array([fts1,fts2])

plot_file = os.path.join("../output/features/figures", "scores_"+"chi2"+"_491_dif_samp"+".pdf")
score_compare_plot(scores, features, labels=['39 samples', '941 samples'], base_score=1, ft_number=10, plt_file = plot_file)

# general experiment for Featuresorter class

    
case = "_39_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

# chi2-test, select out the 30 percents of features that are more correlated with target than other features
stf = FeatureSorter(score_function=scr_func, percent=10)
stf.fit(dataset, malicious) # train the object

sel_fts = stf.selected_fts # selected features
scores = stf.scores # the scores for all the features
sorted_fts = stf.sorted_fts # all the features in decending order

# Write plot to file
plot_file = os.path.join("../output/features/figures", "ranked_features"+case+tail+".pdf")
stf.plot_ft_scores(plot_file) # plot the scores for selected features

# Write scores
score_file = os.path.join("../output/features", "scores"+case+tail+".txt")
stf.write_scores_to_txt(score_file) 

# Write reduced data-set
data_file = os.path.join(programs_path, "sorted_data"+case+tail+".txt")
stf.write_newdata_to_txt(data_file)

# Write sorted features
features_file = os.path.join(programs_path, "sorted_fts"+case+tail+".txt")
stf.write_sorted_features(features_file)

# compare perceptron with correlation tests

if base_sc == 0:
    base = "_base_corr_test"
else:
    base = "_base_percep"

# extract dataset    
case = "_39_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

stf = FeatureSorter(score_function=scr_func, percent=10)
stf.fit(dataset, malicious) # train the object

# extract the feature names and feature scores in correlation test
fts1 = np.array([str(ft) for ft in stf.X.columns])
scores1 = stf.scores

# extract the feature names and feature scores in perceptron
percep_fl_path = os.path.realpath("../output/P"+case+"/perceptron_best-sorted-absolute.txt")
fts_p, scores_p = get_best_fts_perceptron(500,percep_fl_path)

scores2 = []
fts2 = []

# To make scores1 and score2 be same dimension, based on scores1
for ft in fts1:
    if ft in fts_p:
        fts2.append(ft)
        scores2.append(scores_p[fts_p == ft])
        

scores = np.array([scores1,scores2])
features = np.array([fts1,fts2])

# write the plot to file
plot_file = os.path.join("../output/features/figures", "scores_"+tail+"_perceptron"+case+base+".pdf")
score_compare_plot(scores, features, labels=[tail, 'perceptron'], base_score=base_sc, ft_number=10, plt_file = plot_file)

# extract dataset    
case = "_941_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

stf = FeatureSorter(score_function=scr_func, percent=10)
stf.fit(dataset, malicious) # train the object

# extract the feature names and feature scores in correlation test
fts1 = np.array([str(ft) for ft in stf.X.columns])
scores1 = stf.scores

# extract the feature names and feature scores in perceptron
percep_fl_path = os.path.realpath("../output/P"+case+"/perceptron_best-sorted-absolute.txt")
fts_p, scores_p = get_best_fts_perceptron(500,percep_fl_path)

scores2 = []
fts2 = []

# To make scores1 and score2 be same dimension, based on scores1
for ft in fts1:
    if ft in fts_p:
        fts2.append(ft)
        scores2.append(scores_p[fts_p == ft])
        

scores = np.array([scores1,scores2])
features = np.array([fts1,fts2])

# write the plot to file
plot_file = os.path.join("../output/features/figures", "scores_"+tail+"_perceptron"+case+base+".pdf")
score_compare_plot(scores, features, labels=[tail, 'perceptron'], base_score=base_sc, ft_number=10, plt_file = plot_file)

# compare the top features for chi2 and f test

# extract dataset    
case = "_39_491"
dataset, malicious = get_data(programs_path, data_file="allstats_norm"+case+".txt")

degree = []
for fts_perc in np.arange(5,95):
    # chi2-test, select out the 50 percents of features that are more correlated with target than other features
    stf1 = FeatureSorter(score_function=0, percent=fts_perc)
    stf1.fit(dataset, malicious) # train the object
    
    # chi2-test, select out the 50 percents of features that are more correlated with target than other features
    stf2 = FeatureSorter(score_function=1, percent=fts_perc)
    stf2.fit(dataset, malicious) # train the object
    
    fts1 = stf1.selected_fts
    fts2 = stf2.selected_fts
    count = 0
    for ft in fts1:
        if ft in fts2:
            count += 1
    degree.append(count/len(fts1))  
    
fig, ax = plt.subplots()
ax.set_ylabel("Match Percentage", fontsize=14)
ax.set_xlabel("Selected Feature Percentage", fontsize=14)
ax.plot(np.arange(5,95), degree)
ax.set_title("How well Chi2 test and F test match",fontsize=14)

filename = os.path.realpath("../output/features/figures/chi2_f_test_match.pdf")
plt.savefig(filename)

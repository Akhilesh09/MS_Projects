#!/usr/bin/env python3

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from utilities.data_parser import DataParser
from sklearn.decomposition import PCA
from project import Project

### Problem definition ###
# Problem and paths
case = "941_486"
data_path = os.path.join("..", "parsed_data", "allstats_941_486.txt")
programs_path = os.path.join("..", "parsed_data", "programsnames_941.txt")
outdir = os.path.join("..", "report", "figures")

# Prepare data
parser = DataParser()
X = parser.parse_datafile(data_path, programs_path)

pca = PCA(whiten=False)
pca.fit(X.values)
svals = pca.singular_values_

fig, ax = plt.subplots()
ax.set_xlabel("Component Number", fontsize=14)
ax.set_ylabel("Relative Singular Value", fontsize=14)
ax.semilogy(svals/svals[0])
plt.grid()
plt.tight_layout()

filename = os.path.join(outdir, "pca.pdf")
plt.savefig(filename, bbox_inches="tight")
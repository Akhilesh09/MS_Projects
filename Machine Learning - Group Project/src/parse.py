#!/usr/bin/env python3

import os
import pandas as pd
from utilities.data_parser import DataParser

# Define directories
data_dir = os.path.join("..", "raw_data", "newdata")
programs_path = os.path.join(data_dir, "programsnames.txt")
features_path = os.path.join(data_dir, "features_573.txt")
data_file = "stats.txt"
outdir = os.path.join("..", "parsed_data")
case = "941_486"

# Define outputs
outfile = "_".join(["allstats", case])
outpath = os.path.join(outdir, outfile) + ".txt"

# Init parser and parse
parser = DataParser()
parser.parse_dataset(
    features_path, programs_path, data_file
)
parser.write_allstats(outpath)

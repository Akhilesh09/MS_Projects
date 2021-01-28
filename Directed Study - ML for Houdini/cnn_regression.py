# USAGE
# python cnn_regression.py --dataset Houses-dataset/Houses\ Dataset/

# import the necessary packages
from tensorflow.keras.optimizers import Adam
from sklearn.model_selection import train_test_split
from data import datasets
from data import models
import numpy as np
import argparse
import locale
import os
import autokeras as ak

# construct the argument parser and parse the arguments
# ap = argparse.ArgumentParser()
# ap.add_argument("-d", "--dataset", type=str, required=True,
# 	help="path to input dataset of house images")
# args = vars(ap.parse_args())

# construct the path to the input .txt file that contains information
# on each house in the dataset and then load the dataset
print("[INFO] loading attributes...")
# inputPath = os.path.sep.join([args["dataset"], "HousesInfo.txt"])
# df = datasets.load_house_attributes(inputPath)

df_all,attr_names = datasets.load_params()

# load the house images and then scale the pixel intensities to the
# range [0, 1]
print("[INFO] loading images...")
images = datasets.load_images()
images = images / 255.0

# for i in range(len(df_all[0])):
i=0
df=[]
for ele in df_all:
	df.append(float(ele[i]))
df=np.array(df)

# print("df:",df)

shuffle_index = np.random.permutation(len(df))
df = df[shuffle_index]
images = images[shuffle_index]

# partition the data into training and testing splits using 75% of
# the data for training and the remaining 25% for testing
(trainAttrX, testAttrX, trainImagesX, testImagesX) = df[:4000],df[4000:],images[:4000],images[4000:]

# find the largest house price in the training set and use it to
# scale our house prices to the range [0, 1] (will lead to better
# training and convergence)
maxVal = max(trainAttrX)
# print(type(trainAttrX[0]))
trainAttrX = trainAttrX/ maxVal

reg = ak.ImageRegressor(
	overwrite=True,
	max_trials=3)
# Feed the image regressor with training data.
reg.fit(trainImagesX, trainAttrX, epochs=20,validation_split=0.2)

model = reg.export_model()

try:
	model.save("autokeras_"+str(i), save_format="tf")
except:
	model.save("autokeras_"+str(i)+".h5")
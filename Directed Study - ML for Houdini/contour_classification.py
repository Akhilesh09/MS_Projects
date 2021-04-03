# USAGE
# python cnn_regression.py --dataset Houses-dataset/Houses\ Dataset/

# import the necessary packages
import tensorflow as tf
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.applications import MobileNet
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense,Flatten
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.utils import to_categorical
from tensorflow.keras import Input
from sklearn.model_selection import train_test_split
from tensorflow.python.keras.applications.resnet import ResNet101
from data import datasets_contours
import numpy as np
import os
import logging
# import autokeras as ak

# construct the argument parser and parse the arguments
# ap = argparse.ArgumentParser()
# ap.add_argument("-d", "--dataset", type=str, required=True,
# 	help="path to input dataset of house images")
# args = vars(ap.parse_args())

def main(_):

	# construct the path to the input .txt file that contains information
	# on each house in the dataset and then load the dataset
	print("[INFO] loading attributes...")
	# inputPath = os.path.sep.join([args["dataset"], "HousesInfo.txt"])
	# df = datasets_contours.load_house_attributes(inputPath)

	df_all,attr_names = datasets_contours.load_params()

	# load the house images and then scale the pixel intensities to the
	# range [0, 1]
	print("[INFO] loading images...")
	images,indices = datasets_contours.load_images()
	# images = images / 255.0
	mean_val=np.mean(images)
	std_val=np.std(images)
	images=(images-mean_val)/std_val
	df_all=df_all[indices]

	# print(len(attr_names))
	for i in range(len(attr_names)):
	# i=0
		df=[]
		for ele in df_all:
			df.append(float(ele[i]))
		df=np.array(df)

		df_unique=np.sort(np.unique(df))

		for k in range(len(df_unique)):
			for j in range(len(df)):
				if(df[j]==df_unique[k]):
					df[j]=k
		print("total=",len(images))

		shuffle_index = np.random.permutation(len(df))
		df = df[shuffle_index]
		images = images[shuffle_index]
		trainSize=int(0.8*len(df))

		# # partition the data into training and testing splits using 75% of
		# # the data for training and the remaining 25% for testing
		(trainAttrX, testAttrX, trainimagesX, testimagesX) = df[:trainSize],df[trainSize:],images[:trainSize],images[trainSize:]

		# # find the largest house price in the training set and use it to
		# # scale our house prices to the range [0, 1] (will lead to better
		# # training and convergence)
		# maxVal = max(trainAttrX)
		# # print(type(trainAttrX[0]))
		# trainAttrX = trainAttrX/ maxVal

		# print(trainimagesX.shape)

		checkpoint=ModelCheckpoint(filepath=attr_names[i]+".h5",monitor="val_loss",mode="min",save_best_only=True)

		input_shape=Input((32,32,3))
		res_model=MobileNet(include_top=False,weights="imagenet",input_tensor=input_shape,classes=len(df_unique))
		model = Sequential()
		model.add(res_model)
		model.add(Flatten())
		model.add(Dense(len(df_unique), activation='softmax'))
		model.compile(loss='categorical_crossentropy',optimizer=Adam(lr=5e-4))
		model.fit(trainimagesX,to_categorical(trainAttrX),batch_size=10,epochs=100,validation_split=0.2,callbacks=[checkpoint])

		# clf = ak.ImageClassifier(overwrite=True, max_trials=3)
		# # Feed the structured data classifier with training data.
		# clf.fit(trainimagesX, trainAttrX, epochs=10,validation_split=0.1)

		# model = clf.export_model()

		try:
			model.save(attr_names[i], save_format="tf")
		except:
			model.save(attr_names[i]+".h5")

if __name__ == "__main__":
	tf.compat.v1.disable_eager_execution()
	logging.disable(logging.WARNING)
	os.environ["CUDA_VISIBLE_DEVICES"] = "0"
	tf.compat.v1.app.run()
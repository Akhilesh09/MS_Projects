# USAGE
# python cnn_regression.py --dataset Houses-dataset/Houses\ Dataset/

# import the necessary packages
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.applications import ResNet50
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense,Flatten
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.utils import to_categorical
from tensorflow.keras import Input
from sklearn.model_selection import train_test_split
from data import datasets_contours
import numpy as np
import argparse
import os
import json


def run_models(img_name):

	# construct the argument parser and parse the arguments
	# ap = argparse.ArgumentParser()
	# ap.add_argument("-i", "--image_name", type=str, required=True,
	# 	help="input image")
	# args = vars(ap.parse_args())

	# construct the path to the input .txt file that contains information
	# on each house in the dataset and then load the dataset
	# print("[INFO] loading attributes...")

	df_all,attr_names = datasets_contours.load_params()
	print(attr_names)

	# load the house images and then scale the pixel intensities to the
	# range [0, 1]
	dirname = os.path.dirname(__file__)
	filename = os.path.join(dirname, 'imagess')

	# print("[INFO] loading images...")
	images,indices = datasets_contours.load_images()
	# images = images / 255.0
	mean_val=np.mean(images)
	std_val=np.std(images)
	images=(images-mean_val)/std_val
	df_all=df_all[indices]

	out_file=open("out.txt","w")
	out_text={}

	for i in range(len(df_all[0])):
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


		print("[INFO] testing on image "+img_name+"...")


		img_index=int(img_name.split('.')[0])-1
		image_data = images[img_index]

		from tensorflow.keras.models import load_model

		try:
			loaded_model = load_model(attr_names[i], custom_objects=ak.CUSTOM_OBJECTS)
		except:
			loaded_model = load_model(attr_names[i]+".h5")

		# Predict with the best model.
		img = np.expand_dims(image_data,0)
		predicted_y = list(loaded_model.predict(img))
		
		print("Predicted Attribute",str(i))
		out_text[str(attr_names[i])]=str(df_unique[np.argmax(predicted_y)])

	json_dump=json.dumps(out_text)
	return json_dump
# out_file.write(str(json_dump))
# out_file.close()
# run_models("170.png")

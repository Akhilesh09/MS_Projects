# USAGE
# python cnn_regression.py --dataset Houses-dataset/Houses\ Dataset/

# import the necessary packages
from numpy.core.numeric import indices
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
import cv2
import os
import json


def run_models(img_name):

	f=open("data.out",'rb')
	df_all=np.load(f)
	f.close()

	f=open("attr.out",'rb')
	attr_names=np.load(f)
	f.close()

	f=open("indices.out",'rb')
	indices=np.load(f)
	f.close()
	
	path=os.path.join('data/images26k/',img_name)
	
	img= cv2.imread(path)
	blurred = cv2.GaussianBlur(img, (5, 5), 0)
	gray = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)
	lab = cv2.cvtColor(blurred, cv2.COLOR_BGR2LAB)
	thresh = cv2.threshold(gray, 60, 255, cv2.THRESH_BINARY)[1]
	_ , contours , _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
	cv2.drawContours(img, contours, -1, (255, 255, 255), 3)
	image_data=img
	
	mean_val=np.mean(image_data)
	std_val=np.std(image_data)
	image_data=(image_data-mean_val)/std_val

	out_text={}
	img_index=int(img_name.split('.')[0])-1
	img_index=int(np.where(indices==img_index)[0])

	mispredictions={}
	correct_values={}

	for i in range(len(df_all[0])):
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

		from tensorflow.keras.models import load_model

		loaded_model = load_model(os.path.join('26k_models/',attr_names[i])+".h5")

		# Predict with the best model.
		img = np.expand_dims(image_data,0)
		predicted_y = list(loaded_model.predict(img))
		
		print("Predicted Attribute",str(i+1))
		out_text[str(attr_names[i])]=str(df_unique[np.argmax(predicted_y)])

		if(df_all[img_index][i].astype(float)!=df_unique[np.argmax(predicted_y)]):
			mispredictions[str(attr_names[i])] = str(df_unique[np.argmax(predicted_y)])
			correct_values[str(attr_names[i])] = str(df_all[img_index][i])

	json_dump=json.dumps(out_text)

	print('Predicted ',str(10-len(mispredictions)),'/10 attributes correctly')

	for attr in mispredictions:
		print('Mipredicted ',attr,'value as ',mispredictions[attr],' instead of ',correct_values[attr])

	return json_dump

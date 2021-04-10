# USAGE
# python cnn_regression.py --dataset floorplans-dataset/floorplans\ Dataset/

# import the necessary packages
import tensorflow as tf
from tensorflow.keras.optimizers import Adam
from tensorflow.keras.applications import MobileNet
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense,Flatten
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras.utils import to_categorical
from tensorflow.keras import Input
from data import datasets_contours
import numpy as np
import os
import logging

def main(_):

	# construct the path to the input .txt file that contains information
	# on each floorplan in the dataset and then load the dataset
	print("[INFO] loading attributes...")

	df_all,attr_names = datasets_contours.load_params()

	# load the floorplan images 
	print("[INFO] loading images...")
	images,indices = datasets_contours.load_images()
	mean_val=np.mean(images)
	std_val=np.std(images)
	images=(images-mean_val)/std_val
	df_all=df_all[indices]
	# f=open("indices.out",'wb')
	# np.save(f,indices)
	# f.close()
	# f=open("data.out",'wb')
	# np.save(f,df_all)
	# f.close()
	# f=open("attr.out",'wb')
	# np.save(f,attr_names)
	# f.close()
	# print("data saved")
	# input()
	for i in range(9,len(attr_names)):
		df=[]
		for ele in df_all:
			df.append(float(ele[i]))
		df=np.array(df)

		df_unique=np.sort(np.unique(df))

		for k in range(len(df_unique)):
			for j in range(len(df)):
				if(df[j]==df_unique[k]):
					df[j]=k
					

		# shuffle_index = np.random.permutation(len(df))
		# np.savetxt('test.out', shuffle_index, delimiter=',')
		shuffle_index=np.loadtxt('shuffle_indices.out', delimiter=',',dtype=int)
		df = df[shuffle_index]
		images = images[shuffle_index]
		trainSize=int(0.8*len(df))

		# # partition the data into training, testing and validation splits using 80% of
		# # the data for training,10% for testing and 10% for validation
		(trainAttrX, testAttrX,validAttrX, trainimagesX, testimagesX, validimagesX) = df[:trainSize],df[trainSize:int(0.9*len(df))],df[int(0.9*len(df)):],images[:trainSize],images[trainSize:int(0.9*len(df))],images[int(0.9*len(df)):]

		checkpoint=ModelCheckpoint(filepath=attr_names[i]+".h5",monitor="val_loss",mode="min",save_best_only=True)

		input_shape=Input((32,32,3))
		res_model=MobileNet(include_top=False,weights="imagenet",input_tensor=input_shape,classes=len(df_unique))
		model = Sequential()
		model.add(res_model)
		model.add(Flatten())
		model.add(Dense(len(df_unique), activation='softmax'))
		model.compile(loss='categorical_crossentropy',optimizer=Adam(lr=5e-4))
		model.fit(trainimagesX,to_categorical(trainAttrX),batch_size=10,epochs=100,validation_data=(validimagesX,to_categorical(validAttrX)),callbacks=[checkpoint])

		try:
			model.save(attr_names[i], save_format="tf")
		except:
			model.save(attr_names[i]+".h5")

if __name__ == "__main__":
	tf.compat.v1.disable_eager_execution()
	logging.disable(logging.WARNING)
	os.environ["CUDA_VISIBLE_DEVICES"] = "0"
	tf.compat.v1.app.run()
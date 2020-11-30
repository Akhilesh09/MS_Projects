import os
import pickle
import numpy as np
import tensorflow as tf

"""This script implements the functions for reading data.
"""

def load_data(data_dir):
	"""Load the CIFAR-10 dataset.

	Args:
		data_dir: A string. The directory where data batches
			are stored.

	Returns:
		x_train: An numpy array of shape [50000, 3072].
			(dtype=np.float32)
		y_train: An numpy array of shape [50000,].
			(dtype=np.int32)
		x_test: An numpy array of shape [10000, 3072].
			(dtype=np.float32)
		y_test: An numpy array of shape [10000,].
			(dtype=np.int32)
	"""
	
	train_batches_data=[]
	train_batches_labels=[]
	for batch in os.listdir(data_dir):
		path=os.path.join(data_dir,batch)
		if(len(batch.split("."))==1):
			batch_f=open(path,"rb")
			batch=pickle.load(batch_f,encoding='bytes')
			train_batches_data.append(batch[b'data'])
			train_batches_labels.append(batch[b'labels'])
	x_test=train_batches_data.pop(-1)
	y_test=train_batches_labels.pop(-1)
	
	x_train= np.reshape(train_batches_data,(50000,3072))
	y_train= np.reshape(train_batches_labels,(50000,))

	y_train_new=np.zeros((50000,10))
	y_test_new= np.zeros((10000,10))

	#converting labels to one-hot encoding for mixup training

	for i in range(len(y_train)):
		temp=tf.keras.utils.to_categorical(y_train[i], num_classes=10)
		y_train_new[i]=temp

	for i in range(len(y_test)):
		temp=tf.keras.utils.to_categorical(y_test[i], num_classes=10)
		y_test_new[i]=temp

	return x_train, y_train_new, x_test, y_test_new



def load_testing_images(data_dir):
	"""Load the images in private testing dataset.

	Args:
		data_dir: A string. The directory where the testing images
		are stored.

	Returns:
		x_test: An numpy array of shape [N, 32, 32, 3].
			(dtype=np.float32)
	"""

	train_batches_data=[]
	train_batches_labels=[]
	for batch in os.listdir(data_dir):
		if((batch.split("."))[-1]=="npy"):
			path=os.path.join(data_dir,batch)
			batch_f=open(path,"rb")
			x_test=np.load(path)

	return x_test
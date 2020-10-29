import os
import pickle
import numpy as np

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

	### YOUR CODE HERE

	train_batches_data=[]
	train_batches_labels=[]
	for batch in os.listdir(data_dir):
		path=os.path.join(data_dir,batch)
		if(len(batch.split("."))==1):
			batch_f=open(path,"rb")
			batch=pickle.load(batch_f,encoding='bytes')
			train_batches_data.append(batch[b'data'])
			train_batches_labels.append(batch[b'labels'])
	test_batch_data=train_batches_data.pop(-1)
	test_batch_labels=train_batches_labels.pop(-1)
	
	x_train= np.reshape(train_batches_data,(50000,3072))
	y_train= np.reshape(train_batches_labels,(50000,))
	x_test= np.reshape(test_batch_data,(10000,3072))
	y_test= np.reshape(test_batch_labels,(10000,))

	### END CODE HERE

	return x_train, y_train, x_test, y_test

def train_valid_split(x_train, y_train, split_index=45000):
	"""Split the original training data into a new training dataset
	and a validation dataset.

	Args:
		x_train: An array of shape [50000, 3072].
		y_train: An array of shape [50000,].
		split_index: An integer.

	Returns:
		x_train_new: An array of shape [split_index, 3072].
		y_train_new: An array of shape [split_index,].
		x_valid: An array of shape [50000-split_index, 3072].
		y_valid: An array of shape [50000-split_index,].
	"""
	x_train_new = x_train[:split_index]
	y_train_new = y_train[:split_index]
	x_valid = x_train[split_index:]
	y_valid = y_train[split_index:]

	return x_train_new, y_train_new, x_valid, y_valid


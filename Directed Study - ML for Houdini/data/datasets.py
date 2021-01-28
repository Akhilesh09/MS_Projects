# import the necessary packages
from PIL import Image
from sklearn.preprocessing import LabelBinarizer
from sklearn.preprocessing import MinMaxScaler

import numpy as np
import glob
import cv2
import os
import matplotlib

def load_params():
	dirname = os.path.dirname(__file__)
	filename = os.path.join(dirname, 'data.txt')
	params_file=open(filename,'r')
	params=[]
	for line in params_file:
		params.append(line.strip().split('_wedge_')[-1].split('_'))
		
	data_values=[]
	param_names=[]
	for param in params:
		param_values=[]
		for i in range(0,len(param),3):
			to_join=list([param[i],param[i+1]])
			param_name=' '.join(to_join)
			if param_name not in param_names:
				param_names.append(param_name)
			param_values.append(param[i+2])
		data_values.append(param_values)


	return data_values,param_names


def load_images():
	images=[]
	for j in range(5000):
		images.append(0)

	dirname = os.path.dirname(__file__)
	filename = os.path.join(dirname, 'images')
	data=os.listdir(filename)
	data_size=len(data)

	for image_file in (data):
		path=os.path.join(filename,image_file)
		from matplotlib import image
		image_data = image.imread(path)
		img_index=int(image_file.split('.')[0])
		images[img_index-1]=image_data

	images=np.array(images)

	return images

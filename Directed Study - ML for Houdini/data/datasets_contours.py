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
	filename = os.path.join(dirname, 'floors.txt')
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
	data_values=np.array(data_values)
	return data_values,param_names


def load_images():
	images=np.zeros((19682,32,32,3))
	indices=[]

	dirname = os.path.dirname(__file__)
	filename = os.path.join(dirname, 'imagess')
	data=os.listdir(filename)
	data_size=len(data)

	for i in range(data_size):
		path=os.path.join(filename,data[i])
		img= cv2.imread(path)
		blurred = cv2.GaussianBlur(img, (5, 5), 0)
		gray = cv2.cvtColor(blurred, cv2.COLOR_BGR2GRAY)
		lab = cv2.cvtColor(blurred, cv2.COLOR_BGR2LAB)
		thresh = cv2.threshold(gray, 60, 255, cv2.THRESH_BINARY)[1]
		# find contours in the thresholded image
		# cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL,
		# 	cv2.CHAIN_APPROX_SIMPLE)
		_ , contours , _ = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
		cv2.drawContours(img, contours, -1, (255, 255, 255), 3)
		img_index=int(data[i].split('.')[0])
		indices.append(img_index-1)
		# cv2.imshow('Contours', img)
		# cv2.waitKey(0)
		# cv2.destroyAllWindows()
		images[img_index-1]=img

	indices=np.array(indices)

	indices=np.sort(indices)
	images=images[indices]
	
	# images=images[images!=0]
	
	return images,indices

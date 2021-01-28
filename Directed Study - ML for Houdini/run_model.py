# USAGE
# python cnn_regression.py --dataset Houses-dataset/Houses\ Dataset/

# import the necessary packages
from tensorflow.keras.optimizers import Adam
from sklearn.model_selection import train_test_split
from pyimagesearch import datasets
from pyimagesearch import models
import numpy as np
import autokeras as ak
import json

# construct the path to the input .txt file that contains information
# on each house in the dataset and then load the dataset
print("[INFO] loading attributes...")

df_all,attr_names = datasets.load_params()

# load the house images and then scale the pixel intensities to the
# range [0, 1]
print("[INFO] loading images...")
images = datasets.load_images()
images = images / 255.0

out_file=open("out.txt","w")
out_text={}

shuffle_index = np.random.permutation(len(images))

for i in range(len(df_all[0])):
# i=0
	df=[]
	for ele in df_all:
		df.append(float(ele[i]))
	df=np.array(df)

	
	df = df[shuffle_index]
	images = images[shuffle_index]

	# partition the data into training and testing splits using 75% of
	# the data for training and the remaining 25% for testing
	(trainAttrX, testAttrX, trainImagesX, testImagesX) = df[:4000],df[4000:],images[:4000],images[4000:]

	maxVal = max(trainAttrX)

	rand_num=np.random.choice(np.random.permutation(len(testImagesX)))
	testAttr=testAttrX[rand_num]

	from tensorflow.keras.models import load_model

	try:
		loaded_model = load_model("autokeras_"+str(i), custom_objects=ak.CUSTOM_OBJECTS)
	except:
		loaded_model = load_model("autokeras_"+str(i)+".h5")

	# Predict with the best model.
	img = np.expand_dims(testImagesX[rand_num],0)
	predicted_y = loaded_model.predict(img)

	print(predicted_y*maxVal,testAttrX[rand_num])

	out_text[str(attr_names[i])]=str(predicted_y[0][0]*maxVal)

json_dump=json.dumps(out_text)
out_file.write(str(json_dump))
out_file.close()

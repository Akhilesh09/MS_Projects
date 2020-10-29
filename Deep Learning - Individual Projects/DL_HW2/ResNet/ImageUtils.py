import numpy as np

"""This script implements the functions for data augmentation
and preprocessing.
"""

def parse_record(record, training):
	"""Parse a record to an image and perform data preprocessing.

	Args:
		record: An array of shape [3072,]. One row of the x_* matrix.
		training: A boolean. Determine whether it is in training mode.

	Returns:
		image: An array of shape [32, 32, 3].
	"""
	# Reshape from [depth * height * width] to [depth, height, width].
	# depth_major = tf.reshape(record, [3, 32, 32])
	depth_major = record.reshape((3, 32, 32))

	# Convert from [depth, height, width] to [height, width, depth]
	# image = tf.transpose(depth_major, [1, 2, 0])
	image = np.transpose(depth_major, [1, 2, 0])

	image = preprocess_image(image, training)

	return image


def preprocess_image(image, training):
	"""Preprocess a single image of shape [height, width, depth].

	Args:
		image: An array of shape [32, 32, 3].
		training: A boolean. Determine whether it is in training mode.

	Returns:
		image: An array of shape [32, 32, 3].
	"""
	if training:
		### YOUR CODE HERE
		# Resize the image to add four extra pixels on each side.
		# image = tf.image.resize_image_with_crop_or_pad(image, 32 + 8, 32 + 8)
		new_image=np.zeros((image.shape[0]+8,image.shape[1]+8,3))
		for i in range(image.shape[0]):
			for j in range(image.shape[1]):
				new_image[i+4][j+4]=image[i][j]

		image=new_image

		### END CODE HERE

		### YOUR CODE HERE
		# Randomly crop a [32, 32] section of the image.
		# image = tf.random_crop(image, [32, 32, 3])
		import random
		num=random.randint(0,8)
		image=image[num:num+32,num:num+32]
		# HINT: randomly generate the upper left point of the image
		

		### END CODE HERE

		### YOUR CODE HERE
		# Randomly flip the image horizontally.
		# image = tf.image.random_flip_left_right(image)
		import random
		num=random.uniform(0,1)
		if(num>0.5):
			image=np.flip(image,0)
		

		### END CODE HERE

	### YOUR CODE HERE
	# Subtract off the mean and divide by the standard deviation of the pixels.
	# image = tf.image.per_image_standardization(image)
	mean=np.mean(image)
	std=np.std(image)
	image=np.subtract(image,mean)
	image=np.divide(image,std)
	
	
	### END CODE HERE

	return image

# if __name__ == '__main__':
# 	for i in range(10):
# 		preprocess_image(np.ones((32,32,3)),True)


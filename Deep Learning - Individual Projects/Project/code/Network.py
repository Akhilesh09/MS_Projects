import tensorflow
from tensorflow.keras.models import Model
from tensorflow.keras.layers import Dense, Conv2D, BatchNormalization, Activation, Dropout
from tensorflow.keras.layers import AveragePooling2D, Input, Flatten
from tensorflow.keras import backend as K
from tensorflow.keras.regularizers import l2
# import torch

"""This script defines the network.
"""

class MyNetwork(Model):

	def __init__(self, configs):
		super(MyNetwork, self).__init__()
		self.configs = configs
		self.resnet_size = configs["n"]
		self.num_classes = configs["num_classes"]
		self.depth = self.resnet_size * 6 + 2

	def __call__(self):
		'''
		Args:
			inputs: A Tensor representing a batch of input images.
			training: A boolean. Used by operations that work differently
				in training and testing phases such as batch normalization.
		Return:
			The output Tensor of the network.
		'''
		self.model=resnet_v1(input_shape=(32,32,3), depth=self.depth)
		
		return self.model

def resnet_layer(inputs,
				 num_filters=16,
				 kernel_size=3,
				 strides=1,
				 activation='relu',
				 batch_normalization=True,
				 conv_first=True):
	"""2D Convolution-Batch Normalization-Activation stack builder

	# Arguments
		inputs (tensor): input tensor from input image or previous layer
		num_filters (int): Conv2D number of filters
		kernel_size (int): Conv2D square kernel dimensions
		strides (int): Conv2D square stride dimensions
		activation (string): activation name
		batch_normalization (bool): whether to include batch normalization
		conv_first (bool): conv-bn-activation (True) or
			bn-activation-conv (False)

	# Returns
		x (tensor): tensor as input to the next layer
	"""
	
	conv = Conv2D(num_filters,
				  kernel_size=kernel_size,
				  strides=strides,
				  padding='same',
				  kernel_initializer='he_normal',
				  kernel_regularizer=l2(1e-4))

	x = inputs
	if conv_first:
		x = conv(x)
		if batch_normalization:
			x = BatchNormalization()(x)
		if activation is not None:
			x = Activation(activation)(x)
	else:
		if batch_normalization:
			x = BatchNormalization()(x)
		if activation is not None:
			x = Activation(activation)(x)
		x = conv(x)
	return x


def resnet_v1(input_shape, depth, num_classes=10):
	"""ResNet Version 1 Model builder [a]

	Stacks of 2 x (3 x 3) Conv2D-BN-ReLU
	Last ReLU is after the shortcut connection.
	At the beginning of each stage, the feature map size is halved (downsampled)
	by a convolutional layer with strides=2, while the number of filters is
	doubled. Within each stage, the layers have the same number filters and the
	same number of filters.
	Features maps sizes:
	stage 0: 32x32, 16
	stage 1: 16x16, 32
	stage 2:  8x8,  64
	The Number of parameters is approx 0.27M

	# Arguments
		input_shape (tensor): shape of input image tensor
		depth (int): number of core convolutional layers
		num_classes (int): number of classes (CIFAR10 has 10)

	# Returns
		model (Model): Keras model instance
	"""
	if (depth - 2) % 6 != 0:
		raise ValueError('depth should be 6n+2 (eg 20, 32, 44 in [a])')
	# Start model definition.
	num_filters = 16
	num_res_blocks = int((depth - 2) / 6)

	inputs = Input(shape=input_shape)
	x = resnet_layer(inputs=inputs)
	
	# Instantiate the stack of residual units
	for stack in range(3):
		for res_block in range(num_res_blocks):
			strides = 1
			if stack > 0 and res_block == 0:  # first layer but not first stack
				strides = 2  # downsample
			y = resnet_layer(inputs=x,
							 num_filters=num_filters,
							 strides=strides)
			y = resnet_layer(inputs=y,
							 num_filters=num_filters,
							 activation=None)
			if stack > 0 and res_block == 0:  # first layer but not first stack
				# linear projection residual shortcut connection to match
				# changed dims
				x = resnet_layer(inputs=x,
								 num_filters=num_filters,
								 kernel_size=1,
								 strides=strides,
								 activation=None,
								 batch_normalization=False)
			x = tensorflow.keras.layers.add([x, y])
			x = Activation('relu')(x)
		num_filters *= 2
		# Dropout to prevent overfitting
		if(stack>0):
			x=Dropout(rate=0.1*stack)(x)

	# Add classifier on top.
	# v1 does not use BN after last shortcut connection-ReLU
	x = AveragePooling2D(pool_size=8)(x)
	y = Flatten()(x)
	outputs = Dense(num_classes,
					activation='softmax',
					kernel_initializer='he_normal')(y)

	# Instantiate model.
	model = Model(inputs=inputs, outputs=outputs)
	
	return model

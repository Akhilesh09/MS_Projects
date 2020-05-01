#!/bin/bash

# Script for comparing decision tree and perceptron text files across different number of features

for f1 in PF_39_63/*.txt
do
	IFS='/' # / is set as delimiter
	read -ra ADDR <<< $f1 # str is read into an array as tokens separated by IFS
	IFS=' ' # space is set as delimiter
	f2="PF_39_491/${ADDR[-1]}" # Define f2 as same file name from other directory
	echo "-------------------------------------------------------------------------"
	echo "${ADDR[-1]}"
	echo "-------------------------------------------------------------------------"
	diff $f1 $f2
done

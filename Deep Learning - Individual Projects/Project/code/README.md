
The CIFAR-10 and private test data were placed in a folder named "data" inside this folder.

To train the model:
python main.py --mode train --data_dir "data"

To test the model on public test data:
python main.py --mode test --data_dir "data"

To make and store predictions on private test data:
python main.py --mode predict --data_dir "data"

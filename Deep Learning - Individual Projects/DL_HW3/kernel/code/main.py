from DataReader import prepare_data
from model import Model

data_dir = "../data/"
train_filename = "training.npz"
test_filename = "test.npz"

def main():
    # ------------Data Preprocessing------------
    train_X, train_y, valid_X, valid_y, train_valid_X, train_valid_y, test_X, test_y = prepare_data(data_dir, train_filename, test_filename)

    # ------------Kernel Logistic Regression Case------------
    ### YOUR CODE HERE
    # Run your kernel logistic regression model here
    # learning_rate = 0.6
    # max_epoch = 10
    # batch_size =256
    # sigma = 1.0

    # model = Model('Kernel_LR', train_X.shape[0], sigma)
    # model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)

    # import matplotlib.pyplot as plt
    # plt.style.use('seaborn-whitegrid')

    # for j in range(10):
    # for i in range(1,11,1):
    #     learning_rate = 0.1*i
    #     model = Model('Kernel_LR', train_X.shape[0], sigma)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)
    #     plt.plot(i*0.01, valid_score, 'o', color="black")

    # plt.xlabel("LR")
    # plt.ylabel("Validation Score")
    # plt.savefig('KLR_LR2.png')
    # plt.show()

    # for i in range(0,10,1):
    #     model = Model('Kernel_LR', train_X.shape[0], sigma)
    #     print("Batch size:",2**i)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, 2**i)
    #     plt.plot(i*0.1, valid_score, 'o', color="black")

    # plt.xlabel("Sigma")
    # plt.ylabel("Validation Score")
    # plt.savefig('KLR_Sig.png')
    # plt.show()

    # for i in range(1,11,1):
    #     model = Model('Kernel_LR', train_X.shape[0], sigma)
    #     sigma = 0.1*i
    #     print("sigma:",sigma)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)
    #     plt.plot(sigma, valid_score, 'o', color="black")

    # plt.xlabel("Sigma")
    # plt.ylabel("Validation Score")
    # plt.savefig('KLR_Sig.png')
    # plt.show()

    model = Model('Kernel_LR', train_valid_X.shape[0], sigma)
    model.train(train_valid_X, train_valid_y, None, None, max_epoch, learning_rate, batch_size)
    score = model.score(test_X, test_y)
    print("score = {} in test set.\n".format(score))
    ### END YOUR CODE

    # ------------RBF Network Case------------
    ### YOUR CODE HERE
    # Run your radial basis function network model here
    hidden_dim = 256
    learning_rate = 0.5
    max_epoch = 10
    batch_size = 128
    sigma = 1.0

    # model = Model('RBF', hidden_dim, sigma)
    # model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)

    # import matplotlib.pyplot as plt
    # plt.style.use('seaborn-whitegrid')

    # for i in range(1,11,1):
    #     learning_rate = 0.1*i
    #     model = Model('RBF', hidden_dim, sigma)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)
    #     plt.plot(i*0.1, valid_score, 'o', color="black")

    # for i in range(0,10,1):
    #     model = Model('Kernel_LR', train_X.shape[0], sigma)
    #     print("Batch size:",2**i)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, 2**i)
    #     plt.plot(i*0.1, valid_score, 'o', color="black")

    # for i in range(1,11,1):
    #     model = Model('Kernel_LR', train_X.shape[0], sigma)
    #     sigma = 0.1*i
    #     print("sigma:",sigma)
    #     valid_score=model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)
    #     plt.plot(sigma, valid_score, 'o', color="black")

    # plt.xlabel("LR")
    # plt.ylabel("Validation Score")
    # plt.savefig('RBF_BS.png')
    # plt.show()

    model = Model('RBF', hidden_dim, sigma)
    model.train(train_valid_X, train_valid_y, None, None, max_epoch, learning_rate, batch_size)
    score = model.score(test_X, test_y)
    print("score = {} in test set.\n".format(score))
    ### END YOUR CODE

    # ------------Feed-Forward Network Case------------
    ### YOUR CODE HERE
    # Run your feed-forward network model here
    hidden_dim = 128
    learning_rate = 0.1
    max_epoch = 10
    batch_size = 128

    # model = Model('FFN', hidden_dim)
    # model.train(train_X, train_y, valid_X, valid_y, max_epoch, learning_rate, batch_size)

    model = Model('FFN', hidden_dim)
    model.train(train_valid_X, train_valid_y, None, None, max_epoch, learning_rate, batch_size)
    score = model.score(test_X, test_y)
    print("score = {} in test set\n".format(score))
    ### END YOUR CODE
    
if __name__ == '__main__':
    main()
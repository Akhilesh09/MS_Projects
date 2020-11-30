import torch
from helper import load_data
from solution import PCA, AE, frobeniu_norm_error
import numpy as np
import os


def test_pca(A, p):
    pca = PCA(A, p)
    Ap, G = pca.get_reduced()
    A_re = pca.reconstruction(Ap)
    # mean centering A
    A=torch.tensor(A)
    ones_matrix=torch.ones((A.shape[1],1),dtype=torch.double)
    A_mean=torch.div(torch.mm(A,ones_matrix),A.shape[1])
    A_centered=(A-torch.mm(A_mean,torch.transpose(ones_matrix,0,1)))
    error = frobeniu_norm_error(A_centered, A_re)
    print('PCA-Reconstruction error for {k} components is'.format(k=p), error)
    return G

def test_ae(A, p):
    model = AE(d_hidden_rep=p)
    model.train(A, A, 128, 900)
    A_re = model.reconstruction(A)
    final_w = model.get_params()
    error = frobeniu_norm_error(A, A_re)
    print('AE-Reconstruction error for {k}-dimensional hidden representation is'.format(k=p), error)
    return final_w

if __name__ == '__main__':
    dataloc = "../data/USPS.mat"
    A = load_data(dataloc)
    A = A.T
    ## Normalize A
    A = A/A.max()

    ### YOUR CODE HERE
    # Note: You are free to modify your code here for debugging and justifying your ideas for 5(f)
    # ps = [50, 100, 150]
    ps = [32,64,128]
    # uncomment for AE with non-linear layers
    # ps = [64] 
    
    for p in ps:
        G = test_pca(A, p)
        final_w = test_ae(A, p)  
    
    # verify relation between G and W    
    # W=torch.tensor(final_w,dtype=torch.float32)
    # G=torch.tensor(G,dtype=torch.float32)
    # OM=torch.mm(torch.transpose(W,0,1),G)
    # OM_transpose=torch.transpose(OM,0,1)
    # result=torch.mm(OM,OM_transpose).cuda().round()
    # print(result)
    ### END YOUR CODE 

import numpy as np
import sys

LONG_LONG_MAX =  9223372036854775807

def genMatrix():
    # python3 MatrixGen.py n m m2

    n = int(sys.argv[1])
    m = int(sys.argv[2])

    mat = np.random.randint(0,1000,size=(n,m))
    print(mat)
    np.savetxt("inp1.txt",mat,fmt='%i')

    m2 = int(sys.argv[3])

    mat2 = np.random.randint(0,1000,size=(m,m2))
    print(mat2)
    np.savetxt("inp2.txt",mat2,fmt='%i')


    matRes = np.matmul(mat,mat2)
    print(matRes)
    print(matRes.shape)
    if (matRes>LONG_LONG_MAX).sum() > 0:
        genMatrix()
    np.savetxt("res.txt",matRes,fmt='%i')

if __name__ == "__main__":
    genMatrix()
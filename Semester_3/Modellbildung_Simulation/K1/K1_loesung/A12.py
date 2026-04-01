import numpy as np # data arrays
#import scipy # scientific computing
#import matplotlib.pyplot as plt

a=3; b=[2,3]; c=np.arange(1, 77+2, 2);

np.savez('test.npz', b=b, c=c)

del a; del b; del c;

data =np.load('test.npz')
data_saved=data.files
print('In der Datei test.npz sind die Variablen: '+str(data_saved)+' gespeichert.')
b=data['b']; c=data['c']
print('')

A = np.loadtxt('magic_matrix.csv', delimiter=',')

summe=np.sum(A)
print('Die Summe ist: '+str(int(summe))+'.')
maxwert=A.max()     
indizes=np.where(A==A.max())
print('Der Maximalwert: '+str(int(maxwert))+' befindet sich in der Zeile '
     +str(int(indizes[0][0]))+' und der Spalte '+str(int(indizes[1][0]))+'.')
minwert=  A.min()     
indizes=np.where(A==A.min())
print('Der Minimalwert: '+str(int(minwert))+' befindet sich in der Zeile '
     +str(int(indizes[0][0]))+' und der Spalte '+str(int(indizes[1][0]))+'.')
print('')

v_1=A[:,2]; v_1=v_1.reshape(A.shape[0],1)
print('v_1= '); print(v_1)
print('')

summe_gr_40=np.sum(A*(A>40))
print('Die Summe (>40) beträgt: '+str(summe_gr_40)+'.')
b=A[3-1:7-1,4-1]
print('b=',b) # Indizierung beginnt bei Null!

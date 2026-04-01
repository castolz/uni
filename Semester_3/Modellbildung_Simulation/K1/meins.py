# -*- coding: utf-8 -*-

import numpy as np
import scipy
import matplotlib as plt
from numpy import pi as pi
import os
'Aufgabe 1' 
a = 2
b = [2,3]
c = np.array([])
n = 1
while(n <= 77):
    c = np.append(c,n,)
    n +=2
np.savez('test.npz', b=b, c=c)

data = np.load('test.npz')

print(data.files)
del a
del b
del c

b = data['b']
c = data['c'] 

'Aufgabe 2'

matrix = np.loadtxt('magic_matrix.csv', delimiter= ',')

s = np.sum(np.sum(matrix))
print(s)
minp = np.argmin(matrix)
mi = matrix.min()
maxip = np.argmax(matrix)
maxi = matrix.max()
min_row, min_col = np.unravel_index(minp, matrix.shape)
max_row, max_col = np.unravel_index(maxip, matrix.shape)
print('minPos= ' , min_row , ' ',  min_col)
print('maxPos= ' , max_row , ' ' , max_col)
print('min= ' , mi)
print('max= ' , maxi)

v_1 = np.array([])
i = 0
while(i<len(matrix)):
    v_1 = np.append(v_1, matrix[i,2],)
    i += 1
print(v_1)
hoch = 0
for row in matrix:
    for x in row:
        if x >= 40:
            hoch += x
print('Summe über 40: ' , hoch)

v_2 = np.array([])
i = 3
while(i<=7):
    v_2 = np.append(v_2,matrix[i,4])
    i += 1
print(v_2)
# -*- coding: utf-8 -*-
"""
Spyder-Editor

Dies ist eine temporäre Skriptdatei.
"""
import numpy as np
import scipy
import matplotlib as plt
from numpy import pi as pi
import os

# Aufgabe 1

a = 3

print(a)

b = [2, 3]

c =np.array([])

for i in range(77):
    if (i % 2) == 1:
        c = np.append(c,i)


np.savez('test.npz', b_var = b, c_var = c)

del a
del b
del c

data = np.load('test.npz')
b = data['b_var']
c = data['c_var']



# Aufgabe 2

a = np.loadtxt('magic_matrix.csv', delimiter = ',')
b = 0
for i in a:
    for j in i:
        b = b +j
print(b)
print('Min bei ', a.min(), ' An Stelle ' , a.argmin())
print('Max bei ', a.max() , 'An Stelle ' , a.argmax())

e = np.array([])
i = 0
while i < 9:
    e = np.append(e, a[i,2])
    i = i+1
print(e)
i = 0 
j = 0
f = 0
while i < 9:
    while j < 9:
        if a[i,j] > 40:
            f += a[i,j]
        i += 1
        j += 1
print(f)
g = np.array([])
i = 0
j = 0
while i < 9:
    while j < 9:
        if i in range(3,7):
            g = np.append(g, a[i,2])
        i += 1
        j += 1
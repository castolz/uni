import numpy as np
import matplotlib.pyplot as plt
#from numpy import pi as pi
from scipy import signal

R=20
L=9e-3
Cap=1000e-6

num=[1/(R*Cap), 0]
den=[1, 1/(R*Cap), 1/(L*Cap)]
sys=signal.TransferFunction(num, den)

t_vec=np.linspace(0,0.25,501)
u_a=2*signal.step(sys, T=t_vec)[1]

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t_vec, u_a, 'black', label='Rechenwerte')
ax.grid()
bbox_props = dict(fc="white", ec="black", lw=1)
ax.text(0.11, 0.13, 'Hallo Welt', size=15, bbox=bbox_props)
ax.legend()
ax.set_xlabel('t/s')
ax.set_ylabel('u_a/V')

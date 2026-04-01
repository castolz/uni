import numpy as np 
#import scipy 
import matplotlib.pyplot as plt
from numpy import pi as pi
from scipy.integrate import solve_ivp

g=9.81
l=1
d=0.3
phi_0=pi/4

t_max=5
t=np.linspace(0, t_max, 101)

def xdot_fkt(t, x, *args):
    xdot= [x[1], -g/l*x[0]-d*x[1]]
    return xdot

x0=[phi_0, 0]

sol = solve_ivp(xdot_fkt, [0, t_max], x0, t_eval=t, method='LSODA', args=(g, l, d))
t=sol.t; x=sol.y;
phi=x[0]; dot_phi=x[1]

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t, phi, 'b')
ax.grid()
ax.set_xlabel('t')
ax.set_ylabel('$\\varphi$') # \ bedeutet Sonderzeichen (z.B.: \n). Um ein \ zu bekommen:'\\'

import numpy as np 
#import scipy 
import matplotlib.pyplot as plt
from numpy import pi as pi
from scipy.integrate import solve_ivp

u_dach=2
f=1
R=20
L=9e-3
Cap=1000e-6

t_max=2
Delta_t=1e-3
t_aequidistant=np.linspace(0, t_max, int(t_max/Delta_t)+1)

A=np.array([[-1/(R*Cap), -1/Cap], 
            [1/L,         0    ]])
B=np.array([[1/(R*Cap)],
               [0]])
C=np.array([[   1,  0],
            [   0,  1],
            [ -1/R, 0]])
D=np.array([[  0],
            [  0],
            [1/R]])

def xdot_fkt(t, x):
    x=np.reshape(x, (2,1))
# Ergänzen Sie hier die Gleichung zur Berechnung der Ableitungen:

    xdot=np.reshape(xdot, (1,2))
    return xdot

u_a_0=0; i_l_0=0 # Anfangswerte
x0=np.array([u_a_0, i_l_0])

sol = solve_ivp(xdot_fkt, [0, t_max], x0, t_eval=t_aequidistant, method='LSODA')
x=sol.y;
# Ergänzen Sie hier die Gleichung zur Berechnung der Ausgangsgrößen

u_a=y[0]; i_c=y[2]-y[1]; i_e=y[2]

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(211)
ax.plot(t_aequidistant, u_a, 'black')
ax.plot(t_aequidistant, i_c, 'r')
ax.grid()
ax.set_ylabel('$u_a, i_c$')
ax = fig.add_subplot(212)
ax.plot(t_aequidistant, 10*i_e, 'red')
ax.plot(t_aequidistant, u_dach*np.sin(2*pi*f*t_aequidistant), 'darkgreen')
ax.set_ylabel('$u_e, 10 \cdot i_e$')
ax.set_xlabel('t')
ax.grid()

# -*- coding: utf-8 -*-
"""
Created on Tue Mar 31 15:17:21 2026

@author: stch1039
"""
import scipy
from scipy.integrate import solve_ivp
import numpy as np
import matplotlib.pyplot as plt


g = 9.81
l = 1
phi_0 = np.pi/4

t_max = 5 
t = np.linspace(0, t_max, 101)

def xdot_fkt(t, x, *args):
    xdot = [x[1], -(g/l)*x[0]]
    return xdot
    
x0 = [phi_0, 0]
sol = solve_ivp(xdot_fkt,[0,t_max], x0, t_eval=t, method='LSODA', args=(g,l))

t = sol.t; x = sol.y;



plt.plot(sol.t,sol.y[0])
plt.show()

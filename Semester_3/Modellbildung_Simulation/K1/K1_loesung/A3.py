import numpy as np 
#import scipy 
import matplotlib.pyplot as plt
#from numpy import pi as pi

x_red=np.array([0, np.pi/2, np.pi, 1.5*np.pi, 2*np.pi, 2.5*np.pi])
y_red=np.array([0, 1, 0,-1, 0, 1])
x_cyan=np.arange(-np.pi/2, 2.5*np.pi+0.1, 0.1)
y_cyan=np.sin(x_cyan+np.pi/4)
x_blue=np.arange(-np.pi/2, 2.5*np.pi+np.pi/18, np.pi/18)
a=0.75*2./np.sqrt(3.); b=a-1.2
y_blue=a*np.sin(x_blue) + b*np.sin(3*x_blue)

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.set_xlim([-np.pi/2, 2.5*np.pi])
ax.set_ylim([-1.2, 1.2])
ax.plot(x_red, y_red, 'r*', linestyle='-.', markersize=30)
ax.plot(x_cyan, y_cyan, 'c', linewidth=5)
ax.plot(x_blue, y_blue, 'b+-', markersize=10)
ax.set_title('Zweidimensionale Funktionen')
ax.set_xlabel('$\gamma_1$')
ax.set_ylabel('Ordinate')
ax.grid()
ax.set_yticks(np.array([-1.2, -1,0, -0.75, -0.5, 0.0, 0.5, 0.75, 1.0, 1.2]))
ax.set_xticks(np.arange(-np.pi/2, 2.5*np.pi + np.pi/2, np.pi/2))
ax.set_xticklabels(np.array(['$-\pi/2$', '$0$','$\pi/2$','$\pi$','$3/2 \pi$','$2 \pi$','$5/2 \pi$']))


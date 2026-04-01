import numpy as np 
from OMPython import ModelicaSystem
import matplotlib.pyplot as plt
import os

modelname='A3'
mod=ModelicaSystem(os.getcwd()+'/'+modelname+'.mo',
  modelname,[os.getcwd()+'/ModSimBib/package.mo'])
mod.simulate()
[t]=mod.getSolutions('time')
[i_L]=mod.getSolutions('inductor1.i')
[u]=mod.getSolutions('vierqst1.u_out')

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t, 10*i_L, 'r')
ax.plot(t, u, 'darkgreen')
ax.grid()
ax.set_xlabel('t/s')
ax.set_ylabel('u/v bzw. 10*i/A')
ax.set_xticks(np.arange(0, 0.015+0.005, 0.005))
fig.tight_layout()

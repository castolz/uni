from OMPython import ModelicaSystem
import matplotlib.pyplot as plt
from numpy import pi as pi
import os

cw=0.35; du=0.1; rho=1.188; m=1; l=1;
d=cw*rho*l*pi*du**2/(8*m)
modelname='pendel_block_lw'
mod=ModelicaSystem(os.getcwd()+'/'+modelname+'.mo',modelname)
mod.setParameters({'gain2.k':-d})
mod.simulate()
[t]=mod.getSolutions('time')
[phi]=mod.getSolutions('integrator2.y')

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t, phi, 'b')
ax.grid()
ax.set_xlabel('t')
ax.set_ylabel('$\\varphi$') # \ bedeutet Sonderzeichen (z.B.: \n). Um ein \ zu bekommen:'\\'



from OMPython import ModelicaSystem
import matplotlib.pyplot as plt
import os

modelname='pendel_block'
mod=ModelicaSystem(os.getcwd()+'/'+modelname+'.mo',modelname)
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

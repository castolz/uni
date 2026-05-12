# -*- coding: utf-8 -*-
"""
Created on Tue Mar 31 16:19:54 2026

@author: stch1039
"""

from OMPython import ModelicaSystem
from pathlib import Path
import matplotlib.pyplot as plt

modelname = 'pendel'
model_file = Path(__file__).with_name("pendel.mo")

mod = ModelicaSystem(str(model_file), modelname)

mod.setSimulationOptions({
    "startTime": 0,
    "stopTime": 5,
    "stepSize": 0.01
})

print(mod.getSimulationOptions())

mod.simulate()

t = mod.getSolutions('time')[0]
phi = mod.getSolutions('phi')[0]

print(t[0], t[-1], len(t))

plt.figure()
plt.plot(t, phi)
plt.xlabel("t [s]")
plt.ylabel("phi [rad]")
plt.grid()
plt.tight_layout()
plt.show()
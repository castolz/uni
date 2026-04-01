# -*- coding: utf-8 -*-
"""
Created on Tue Mar 31 16:19:54 2026

@author: stch1039
"""

from OMPython import ModelicaSystem

modelname = 'pendel'
mod = ModelicaSystem('C:/Users/stch1039/Modellbildung und simulation/Modellbildung_Simulation/Modellbildung_Simulation/K2/eigenlösung/pendel.mo','pendel')
mod.simulate()
[t]=mod.getSolutions('time')
[phi]=mod.getSolutions('phi')
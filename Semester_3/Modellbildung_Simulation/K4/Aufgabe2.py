# -*- coding: utf-8 -*-
"""
Created on Mon May 11 13:37:55 2026

@author: chris
"""

import numpy as np
import matplotlib.pyplot as plt

R = 10
L = 10e-3
Tp = 5e-3
u0 = 300
i0 = 10
duty = 0.2
N = 12

tau = L / R

t_ein = duty * Tp
t_0 = Tp - t_ein
t_0_halbe = t_0 / 2

N1 = round(N * t_0_halbe / Tp)
N2 = round(N * t_ein / Tp)

Delta_t_1 = t_0_halbe / N1
Delta_t_2 = t_ein / N2

# Abschnitt a: Aus-Zeit, Strom fällt
t_a_vec = np.arange(0, t_0_halbe, Delta_t_1)
i_a_vec = i0 * np.exp(-t_a_vec / tau)

# Strom am Ende von Abschnitt a
i_a_end = i0 * np.exp(-t_0_halbe / tau)

# Abschnitt b: Ein-Zeit, Strom steigt gegen u0/R
t_b_local = np.arange(0, t_ein, Delta_t_2)
t_b_vec = t_0_halbe + t_b_local
i_b_vec = u0 / R + (i_a_end - u0 / R) * np.exp(-t_b_local / tau)

# Strom am Ende von Abschnitt b
i_b_end = u0 / R + (i_a_end - u0 / R) * np.exp(-t_ein / tau)

# Abschnitt c: Aus-Zeit, Strom fällt wieder
t_c_local = np.arange(0, t_0_halbe, Delta_t_1)
t_c_vec = t_0_halbe + t_ein + t_c_local
i_c_vec = i_b_end * np.exp(-t_c_local / tau)

# Gesamtvektoren
t_vec = np.concatenate((t_a_vec, t_b_vec, t_c_vec))
i_vec = np.concatenate((i_a_vec, i_b_vec, i_c_vec))

# Spannungsvektor für steps-post
t_u = np.array([0, t_0_halbe, t_0_halbe + t_ein, Tp])
u_vec = np.array([0, u0, 0, 0])

print("t_vec =", t_vec)
print("i_vec =", i_vec)

plt.plot(t_vec * 1000, i_vec, marker="o", label="Strom i(t)")
plt.step(t_u * 1000, u_vec / 10, where="post", label="Spannung u(t) / 10")
plt.xlabel("t in ms")
plt.ylabel("i in A")
plt.grid()
plt.legend()
plt.show()
import numpy as np
import matplotlib.pyplot as plt


def RL_circuit(duty, T_p, N, u_0, i_0, R, L):

    tau = L / R

    t_ein = duty * T_p
    t_0_halbe = (T_p - t_ein) / 2

    # Punktverteilung
    N2 = round(N * duty)

    # gleiche Parität für Symmetrie
    if N2 % 2 != N % 2:
        N2 += 1

    # kleine duty
    if N % 2 == 0:
        N2 = max(N2, 2)
    else:
        N2 = max(N2, 1)

    # große duty
    N2 = min(N2, N - 2)

    N1 = (N - N2) // 2

    Delta_t_1 = t_0_halbe / N1
    Delta_t_2 = t_ein / N2

    # Abschnitt a
    t_a = np.arange(0, N1) * Delta_t_1
    i_a = i_0 * np.exp(-t_a / tau)

    i_a_end = i_0 * np.exp(-t_0_halbe / tau)

    # Abschnitt b
    t_b_local = np.arange(0, N2) * Delta_t_2
    t_b = t_0_halbe + t_b_local

    i_b = u_0 / R + (i_a_end - u_0 / R) * np.exp(-t_b_local / tau)

    i_b_end = u_0 / R + (i_a_end - u_0 / R) * np.exp(-t_ein / tau)

    # Abschnitt c
    t_c_local = np.arange(0, N1) * Delta_t_1
    t_c = t_0_halbe + t_ein + t_c_local

    i_c = i_b_end * np.exp(-t_c_local / tau)

    # Gesamtvektoren
    t = np.concatenate((t_a, t_b, t_c))
    i = np.concatenate((i_a, i_b, i_c))

    return i, t


# Parameter
R = 10
L = 10e-3
u0 = 300


# ---------- Plot 1 ----------
i1, t1 = RL_circuit(0.05, 5e-3, 11, 300, 10, R, L)

duty1 = 0.05
Tp = 5e-3
t_ein1 = duty1 * Tp
t0_1 = (Tp - t_ein1) / 2

t_u1 = [0, t0_1, t0_1 + t_ein1, Tp]
u_vec1 = [0, u0, 0, 0]


# ---------- Plot 2 ----------
i2, t2 = RL_circuit(0.95, 5e-3, 6, 300, 10, R, L)

duty2 = 0.95
t_ein2 = duty2 * Tp
t0_2 = (Tp - t_ein2) / 2

t_u2 = [0, t0_2, t0_2 + t_ein2, Tp]
u_vec2 = [0, u0, 0, 0]


# ---------- Darstellung ----------
fig, ax = plt.subplots(2, 1, figsize=(8, 6))

fig.suptitle("Lösung zu Aufgabe 3")

# oberer Plot
ax[0].plot(t1, i1 * 10, 'rs-')
ax[0].plot(t_u1, u_vec1, 'g', drawstyle='steps-post')

ax[0].set_ylabel("u/V bzw. 10*i/A")
ax[0].set_xlabel("t/s")
ax[0].grid(True)

# unterer Plot
ax[1].plot(t2, i2 * 10, 'rs-')
ax[1].plot(t_u2, u_vec2, 'g', drawstyle='steps-post')

ax[1].set_ylabel("u/V bzw. 10*i/A")
ax[1].set_xlabel("t/s")
ax[1].grid(True)

plt.tight_layout()
plt.show()
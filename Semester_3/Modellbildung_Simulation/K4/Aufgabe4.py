import numpy as np
import matplotlib.pyplot as plt


def RL_circuit(duty, T_p, N, u_0, i_0, R, L):

    tau = L / R

    t_ein = duty * T_p
    t_0_halbe = (T_p - t_ein) / 2

    # -----------------------------
    # Punktverteilung bestimmen
    # -----------------------------
    N2 = round(N * duty)

    # gleiche Parität für Symmetrie
    if N2 % 2 != N % 2:
        N2 += 1

    # minimale Punkte in der Mitte
    if N % 2 == 0:
        N2 = max(N2, 2)
    else:
        N2 = max(N2, 1)

    # minimale Punkte links/rechts
    N2 = min(N2, N - 2)

    N1 = (N - N2) // 2

    Delta_t_1 = t_0_halbe / N1
    Delta_t_2 = t_ein / N2

    # -----------------------------
    # Abschnitt a
    # -----------------------------
    t_a = np.arange(0, N1) * Delta_t_1

    i_a = i_0 * np.exp(-t_a / tau)

    i_a_end = i_0 * np.exp(-t_0_halbe / tau)

    # -----------------------------
    # Abschnitt b
    # -----------------------------
    t_b_local = np.arange(0, N2) * Delta_t_2

    t_b = t_0_halbe + t_b_local

    i_b = u_0 / R + (i_a_end - u_0 / R) * \
          np.exp(-t_b_local / tau)

    i_b_end = u_0 / R + (i_a_end - u_0 / R) * \
              np.exp(-t_ein / tau)

    # -----------------------------
    # Abschnitt c
    # -----------------------------
    t_c_local = np.arange(0, N1) * Delta_t_1

    t_c = t_0_halbe + t_ein + t_c_local

    i_c = i_b_end * np.exp(-t_c_local / tau)

    # -----------------------------
    # Gesamtvektoren
    # -----------------------------
    t = np.concatenate((t_a, t_b, t_c))

    i = np.concatenate((i_a, i_b, i_c))

    return i, t


# =====================================================
# Aufgabe 4
# =====================================================

R = 10
L = 10e-3

T_p = 5e-3
N = 12

u_0 = 300

u_mittel_vec = np.array([90, 150, 210])

# Gesamtvektoren
t_gesamt = np.array([])
i_gesamt = np.array([])

t_u_gesamt = np.array([])
u_gesamt = np.array([])

# Startstrom
i_0 = 30

# =====================================================
# Schleife über alle Zyklen
# =====================================================

for k in range(len(u_mittel_vec)):

    u_mittel = u_mittel_vec[k]

    # duty bestimmen
    duty = u_mittel / u_0

    # RL-Kreis berechnen
    i, t = RL_circuit(
        duty,
        T_p,
        N,
        u_0,
        i_0,
        R,
        L
    )

    # Zeit verschieben
    t_shift = k * T_p

    t = t + t_shift

    # Strom anhängen
    t_gesamt = np.concatenate((t_gesamt, t))

    i_gesamt = np.concatenate((i_gesamt, i))

    # -------------------------------------------------
    # Spannungsverlauf
    # -------------------------------------------------

    t_ein = duty * T_p

    t_0_halbe = (T_p - t_ein) / 2

    t_u = np.array([
        0,
        t_0_halbe,
        t_0_halbe + t_ein,
        T_p
    ]) + t_shift

    u_vec = np.array([
        0,
        u_0,
        0,
        0
    ])

    t_u_gesamt = np.concatenate((t_u_gesamt, t_u))

    u_gesamt = np.concatenate((u_gesamt, u_vec))

    # Endstrom wird Anfangsstrom
    # des nächsten Zyklus
    i_0 = i[-1]

# =====================================================
# Plot
# =====================================================

plt.figure(figsize=(9, 5))

plt.title("Lösung zu Aufgabe 4")

# Strom
plt.plot(
    t_gesamt,
    i_gesamt * 10,
    'r.-',
    linewidth=1
)

# Spannung
plt.plot(
    t_u_gesamt,
    u_gesamt,
    'g',
    drawstyle='steps-post'
)

plt.xlabel("t/s")

plt.ylabel("u/V bzw. 10*i/A")

plt.grid(True)

plt.show()
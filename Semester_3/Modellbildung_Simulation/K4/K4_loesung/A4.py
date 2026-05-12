import numpy as np
import matplotlib.pyplot as plt
#from numpy import pi as pi

u_0=300   # Gleichspannung
R=10
L=10e-3

def RL_circuit(duty, T_p, N, u_0, i_0, R, L):
    T=L/R
    N_1=round((1-duty)/2*(N-1));
    if N_1<1:
        N_1=1
    N_2=(N-1)-2*N_1;
    if N_2<1:
      if N % 2: 
          N_2=2
      else: 
          N_2=1
    N_1=(N-1-N_2)/2; 
    t_ein=duty*T_p; t_0_halbe=(T_p-t_ein)/2;
    Delta_t_1=t_0_halbe/N_1; Delta_t_2=t_ein/N_2;
    
    # Zeit- und Stromvektor:
    t_a_vec=Delta_t_1*np.arange(0,N_1+1)
    t_b_vec=Delta_t_2*np.arange(0,N_2+1)
    t_c_vec=t_a_vec
    i_0_neu=i_0
    i_a_vec=i_0_neu*np.exp(-t_a_vec/T);
    i_8=u_0/R; i_0_neu=i_a_vec[-1]
    i_b_vec=i_8+(i_0_neu-i_8)*np.exp(-t_b_vec/T)
    i_0_neu=i_b_vec[-1]
    i_c_vec=i_0_neu*np.exp(-t_c_vec/T);
    t=np.concatenate((t_a_vec, t_b_vec[1:-1]+t_0_halbe, t_c_vec+t_0_halbe+t_ein))
    i=np.concatenate((i_a_vec, i_b_vec[1:-1], i_c_vec))
    return i, t

def u_verlauf(duty, T_p, u_0):
    t_ein=duty*T_p; t_0_halbe=(T_p-t_ein)/2;
    t_u=np.array([0, t_0_halbe, t_0_halbe+t_ein, T_p])
    u=np.array([0, u_0, 0, 0])
    return u, t_u

T_p=5e-3; N=12
i_0_neu=30
t_0_neu=0

u_mittel_vec=np.array([90, 150, 210])
i_vec=np.array([]); t_vec=np.array([])
u_vec=np.array([]); t_u_vec=np.array([])
for k in range(3):
    t_akt=k*T_p+T_p/2; # Zeitpunkt: In der Mitte des Zyklusses
    u_mittel=u_mittel_vec[k]
    duty=abs(u_mittel/u_0); 
    if u_mittel<0:
        u_const=-u_0
    else:
        u_const=u_0
    i, t=RL_circuit(duty, T_p, N, u_const, i_0_neu, R, L)
    t_vec=np.append(t_vec[0:-1], t+t_0_neu); i_vec=np.append(i_vec[0:-1], i)
    u, t_u = u_verlauf(duty, T_p, u_const)
    t_u_vec=np.append(t_u_vec[0:-1], t_u+t_0_neu); u_vec=np.append(u_vec[0:-1], u)
    i_0_neu=i[-1]
    t_0_neu=t_vec[-1]

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t_vec, 10*i_vec, 'r-+')
ax.plot(t_u_vec, u_vec, 'darkgreen', drawstyle='steps-post')
ax.grid()
ax.set_title('Lösung zu Aufgabe 4')
ax.set_xlabel('t/s')
ax.set_ylabel('u/v bzw. 10*i/A')
ax.set_xticks(np.arange(0, 0.015+0.005, 0.005))



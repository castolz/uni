import numpy as np 
#import scipy 
import matplotlib.pyplot as plt
#from numpy import pi as pi

u_0=300   # Gleichspannung
R=10
L=10e-3

duty=.2; T_p=5e-3; N=12; i_0=10;

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

# Zeit- und Spannungsvektor zum Potten des Spannungsverlaufes:
t_u=np.array([0, t_0_halbe, t_0_halbe+t_ein, T_p])
u=np.array([0, u_0, 0, 0])

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

plt.close('all')
fig=plt.figure(1, figsize=(10,6))
ax = fig.add_subplot(111)
ax.plot(t, 10*i, 'r-s')
ax.plot(t_u, u, 'darkgreen', drawstyle='steps-post')
ax.grid()
ax.set_title('Lösung zu Aufgabe 1')
ax.set_xlabel('t/s')
ax.set_ylabel('u/v bzw. 10*i/A')

import numpy as np
import matplotlib.pyplot as plt
from scipy import optimize

def f_skalar(x): # diese Form wird gebraucht für optimize.minimize
    z=5 * x[0]**4 - 10 * x[0]**2 * x[1] + x[0]**2 + 5 * x[1]**2 - 2*x[0] + 5
    return z

# diese Form ist zwar langsamer, ist aber erforderlich, wenn signal.TransferFunction
    # in der Funktion gebraucht wird
def f(x): 
    z_spalten=x[1].shape[1]
    z_zeilen=x[0].shape[0]
    z=np.zeros((z_zeilen, z_spalten))
    for l in range(z_spalten): # Spalten
        print('Durchlauf: '+str(l+1)+' von '+str(z_spalten))
        for k in range(z_zeilen): # Zeilen
            x_0=x[0,k,l]; x_1=x[1,k,l]
            z[k,l]=5 * x_0**4 - 10 * x_0**2 * x_1 + x_0**2 + 5 * x_1**2 - 2*x_0 + 5
    return z

n = 60
x = np.linspace(-2, 2, n)
y = np.linspace(-2, 4, n)

x_a, y_a = np.meshgrid(x, y)
#Z_skalar=f_skalar(np.array([x_a, y_a]))
Z=f(np.array([x_a, y_a]))

x_min = optimize.minimize(f_skalar, x0=[-1, 2.5])
print('Das Minimum ist an der Stelle x='+str(np.round(x_min.x[0]))+' und y='+str(np.round(x_min.x[1])))

plt.close('all')
fig=plt.figure(1, figsize=(6,6))
ax=fig.add_subplot(111)
ax.contourf(x_a, y_a, Z, 25, alpha=.75, cmap='jet', vmin=np.min(Z), vmax=50)
ax.contour(x_a, y_a, Z, 25, colors='black')
ax.set_aspect('equal')
ax.set_xlabel('$x_0$(x)-Achse')
ax.set_ylabel('$x_1$(y)-Achse')

fig=plt.figure(2, figsize=(8,6))
ax = fig.add_subplot(projection='3d')
# bei rstride=10 würe nur jeder 10te Wert dargestellt werden. Ausprobieren!
ax.plot_surface(x_a, y_a, Z, rstride=1, cstride=1, cmap='jet', vmin=np.min(Z), vmax=10)
# elev und azim auslesen mit ax.elev und ax.azim
# wird auch unten im plot-window angezeigt
# Maus rauf runter verändert elevation
# Maus links rechts verändert azimuth
ax.view_init(azim=69, elev=61)
ax.set_xlabel('$x_0$(x)-Achse')
ax.set_ylabel('$x_1$(y)-Achse')
ax.set_zlabel('Funktionswert: $z$-Achse')

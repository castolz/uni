# 3.1 Rechnungsaufgaben – Sourcefolger mit PMOS-Transistor

## Gegeben

$$
R_S = 330\,\Omega
$$

$$
R_{G1}+R_{G2}=502\,\text{k}\Omega
$$

$$
V_{PLUS}=5\,\text{V}
$$

$$
R_L=1\,\text{k}\Omega
$$

$$
C_1=10\,\mu\text{F}
$$

$$
C_2=10\,\mu\text{F}
$$

$$
V_{th}=-0{,}5\,\text{V}
$$

$$
k_p=0{,}331\,\frac{\text{A}}{\text{V}^2}
$$

Für die Rechnung wird der Transistor-Ausgangswiderstand $r_o$ vernachlässigt.

---

## a) Allgemeiner Ausdruck für den Arbeitspunktstrom $I_{D0}$

Beim Sourcefolger gilt näherungsweise für den Kleinsignal-Ausgangswiderstand:

$$
r_{out}\approx \frac{1}{g_m+\frac{1}{R_S}}
$$

Nach $g_m$ umgestellt:

$$
g_m=\frac{1}{r_{out}}-\frac{1}{R_S}
$$

Für den MOSFET im Arbeitspunkt gilt:

$$
g_m=\sqrt{2\,k_p\,I_{D0}}
$$

Damit folgt:

$$
\boxed{I_{D0}=\frac{1}{2\,k_p}\left(\frac{1}{r_{out}}-\frac{1}{R_S}\right)^2}
$$

---

## b) Arbeitspunktstrom für $r_{out}=16\,\Omega$

Zuerst wird $g_m$ berechnet:

$$
g_m=\frac{1}{16\,\Omega}-\frac{1}{330\,\Omega}
$$

$$
g_m=0{,}05947\,\text{S}
$$

Damit:

$$
I_{D0}=\frac{g_m^2}{2\,k_p}
$$

$$
I_{D0}=\frac{(0{,}05947)^2}{2\cdot 0{,}331}
$$

$$
\boxed{I_{D0}=5{,}34\,\text{mA}}
$$

Die Sourcespannung ergibt sich über den Spannungsabfall an $R_S$.
Da $R_S$ zwischen $V_{PLUS}$ und Source liegt, gilt:

$$
V_{S0}=V_{PLUS}-I_{D0}\cdot R_S
$$

$$
V_{S0}=5\,\text{V}-5{,}34\,\text{mA}\cdot 330\,\Omega
$$

$$
V_{S0}=5\,\text{V}-1{,}76\,\text{V}
$$

$$
\boxed{V_{S0}=3{,}24\,\text{V}}
$$

---

## c) Dimensionierung von $R_{G1}$ und $R_{G2}$

Für den PMOS gilt im Sättigungsbereich:

$$
I_{D0}=\frac{k_p}{2}\left(V_{SG0}-|V_{th}|\right)^2
$$

Nach $V_{SG0}$ umgestellt:

$$
V_{SG0}=|V_{th}|+\sqrt{\frac{2\,I_{D0}}{k_p}}
$$

Einsetzen:

$$
V_{SG0}=0{,}5\,\text{V}+\sqrt{\frac{2\cdot 5{,}34\,\text{mA}}{0{,}331}}
$$

$$
V_{SG0}=0{,}680\,\text{V}
$$

Für den PMOS ist:

$$
V_{SG0}=V_{S0}-V_{G0}
$$

Also:

$$
V_{G0}=V_{S0}-V_{SG0}
$$

$$
V_{G0}=3{,}24\,\text{V}-0{,}680\,\text{V}
$$

$$
V_{G0}=2{,}56\,\text{V}
$$

Der Spannungsteiler liefert:

$$
V_{G0}=V_{PLUS}\cdot \frac{R_{G2}}{R_{G1}+R_{G2}}
$$

Mit

$$
R_{G1}+R_{G2}=502\,\text{k}\Omega
$$

folgt:

$$
R_{G2}=\frac{V_{G0}}{V_{PLUS}}\cdot 502\,\text{k}\Omega
$$

$$
R_{G2}=\frac{2{,}56}{5}\cdot 502\,\text{k}\Omega
$$

$$
\boxed{R_{G2}=256{,}8\,\text{k}\Omega}
$$

$$
R_{G1}=502\,\text{k}\Omega-R_{G2}
$$

$$
\boxed{R_{G1}=245{,}2\,\text{k}\Omega}
$$

---

## d) Kleinsignalspannungsverstärkung

### Ohne Lastwiderstand $R_L$

Allgemeiner Ausdruck:

$$
A_{V0}=\frac{g_m\cdot R_S}{1+g_m\cdot R_S}
$$

Einsetzen:

$$
A_{V0}=\frac{0{,}05947\cdot 330}{1+0{,}05947\cdot 330}
$$

$$
\boxed{A_{V0}=0{,}952}
$$

---

### Mit Lastwiderstand $R_L$

Für das Kleinsignal wird $C_2$ als Kurzschluss betrachtet.
Deshalb liegt $R_L$ wechselspannungsmäßig parallel zu $R_S$:

$$
R_S^*=R_S\parallel R_L
$$

$$
R_S^*=330\,\Omega\parallel 1000\,\Omega
$$

$$
R_S^*=\frac{330\cdot 1000}{330+1000}\,\Omega
$$

$$
R_S^*=248{,}1\,\Omega
$$

Allgemeiner Ausdruck:

$$
A_V=\frac{g_m\cdot (R_S\parallel R_L)}{1+g_m\cdot (R_S\parallel R_L)}
$$

Einsetzen:

$$
A_V=\frac{0{,}05947\cdot 248{,}1}{1+0{,}05947\cdot 248{,}1}
$$

$$
\boxed{A_V=0{,}937}
$$

---

## Endergebnisse

$$
\boxed{I_{D0}=5{,}34\,\text{mA}}
$$

$$
\boxed{V_{S0}=3{,}24\,\text{V}}
$$

$$
\boxed{R_{G1}=245{,}2\,\text{k}\Omega}
$$

$$
\boxed{R_{G2}=256{,}8\,\text{k}\Omega}
$$

$$
\boxed{A_{V0}=0{,}952}
$$

$$
\boxed{A_V=0{,}937}
$$

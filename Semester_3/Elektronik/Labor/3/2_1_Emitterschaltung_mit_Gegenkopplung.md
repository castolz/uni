# 2 Emitterschaltung mit Gegenkopplungswiderstand

## Gegebene Werte

| Größe | Wert |
|---|---:|
| $V_{PLUS}$ | $5\,\mathrm{V}$ |
| $R_C$ | $1\,\mathrm{k\Omega}$ |
| $R_B = R_{B1} + R_{B2}$ | $502\,\mathrm{k\Omega}$ |
| $R_E$ | $100\,\Omega \parallel 330\,\Omega = 76.74\,\Omega$ |
| $R_L$ | $100\,\mathrm{k\Omega}$ |
| $V_{BE0}$ | $0.65\,\mathrm{V}$ |
| $V_{CE,sat}$ | $0.2\,\mathrm{V}$ |
| $\beta$ | $294.3$ |
| $V_A$ | $\infty$ |

---

## a) Allgemeiner Ausdruck für den optimalen Kollektorstrom

Für den maximalen Aussteuerbereich wird die Kollektorspannung im Arbeitspunkt mittig zwischen der oberen Grenze $V_{PLUS}$ und der unteren Grenze $V_E + V_{CE,sat}$ gelegt.

Es gilt:

$$
V_{C0} = V_{PLUS} - I_{C0} \cdot R_C
$$

und

$$
V_E = I_{E0} \cdot R_E
= I_{C0} \cdot \left(1 + \frac{1}{\beta}\right) \cdot R_E
$$

Die optimale Bedingung ist:

$$
V_{C0} = \frac{V_{PLUS} + V_E + V_{CE,sat}}{2}
$$

Damit ergibt sich:

$$
\boxed{I_{C0} =
\frac{V_{PLUS} - V_{CE,sat}}
{2 \cdot R_C + \left(1 + \frac{1}{\beta}\right) \cdot R_E}}
$$

---

## b) Optimaler Arbeitspunkt

Einsetzen:

$$
I_{C0} =
\frac{5\,\mathrm{V} - 0.2\,\mathrm{V}}
{2 \cdot 1000\,\Omega + \left(1 + \frac{1}{294.3}\right) \cdot 76.74\,\Omega}
$$

$$
\boxed{I_{C0} = 2.311\,\mathrm{mA}}
$$

Damit liegt der Wert im geforderten Bereich von $2\,\mathrm{mA}$ bis $2.5\,\mathrm{mA}$.

Die optimale Kollektorspannung ist:

$$
V_{C0} = V_{PLUS} - I_{C0} \cdot R_C
$$

$$
V_{C0} = 5\,\mathrm{V} - 2.311\,\mathrm{mA} \cdot 1\,\mathrm{k\Omega}
$$

$$
\boxed{V_{C0} = 2.689\,\mathrm{V}}
$$

Emitterspannung:

$$
V_E = I_{C0} \cdot \left(1 + \frac{1}{\beta}\right) \cdot R_E
$$

$$
\boxed{V_E = 0.178\,\mathrm{V}}
$$

Untere Ausgangsgrenze:

$$
V_{C,min} = V_E + V_{CE,sat}
$$

$$
V_{C,min} = 0.178\,\mathrm{V} + 0.2\,\mathrm{V}
= 0.378\,\mathrm{V}
$$

Maximaler Aussteuerbereich:

$$
V_{out,max,pp} =
2 \cdot \min \left(
V_{PLUS} - V_{C0},
V_{C0} - (V_E + V_{CE,sat})
\right)
$$

$$
\boxed{V_{out,max,pp} = 4.622\,\mathrm{V}}
$$

---

## c) Widerstandswerte $R_{B1}$ und $R_{B2}$

Die Basisspannung ist:

$$
V_{B0} = V_E + V_{BE0}
$$

$$
V_{B0} = 0.178\,\mathrm{V} + 0.65\,\mathrm{V}
= 0.828\,\mathrm{V}
$$

Der Basisstrom ist:

$$
I_{B0} = \frac{I_{C0}}{\beta}
$$

$$
I_{B0} = \frac{2.311\,\mathrm{mA}}{294.3}
= 7.853\,\mathrm{\mu A}
$$

Für den belasteten Spannungsteiler gilt:

$$
R_{B1} + R_{B2} = 502\,\mathrm{k\Omega}
$$

und am Basisknoten:

$$
\frac{V_{PLUS} - V_{B0}}{R_{B1}}
=
\frac{V_{B0}}{R_{B2}} + I_{B0}
$$

Daraus folgt:

$$
\boxed{R_{B1} = 329.64\,\mathrm{k\Omega}}
$$

$$
\boxed{R_{B2} = 172.36\,\mathrm{k\Omega}}
$$

Kontrolle:

$$
R_{B1} + R_{B2}
= 329.64\,\mathrm{k\Omega} + 172.36\,\mathrm{k\Omega}
= 502\,\mathrm{k\Omega}
$$

---

## d) Kleinsignal-Eingangswiderstand und Ausgangswiderstand

Für den Transistor gilt im Arbeitspunkt:

$$
g_m = \frac{I_{C0}}{V_T}
$$

mit $V_T \approx 26\,\mathrm{mV}$:

$$
g_m = \frac{2.311\,\mathrm{mA}}{26\,\mathrm{mV}}
= 0.0889\,\mathrm{S}
$$

Der differentielle Basis-Emitter-Widerstand ist:

$$
r_\pi = \frac{\beta}{g_m}
$$

$$
r_\pi = \frac{294.3}{0.0889\,\mathrm{S}}
= 3.311\,\mathrm{k\Omega}
$$

Der Eingangswiderstand in die Basis mit Emittergegenkopplung ist:

$$
r_{in,B} = r_\pi + (\beta + 1) \cdot R_E
$$

$$
r_{in,B} =
3.311\,\mathrm{k\Omega}
+ 295.3 \cdot 76.74\,\Omega
= 25.97\,\mathrm{k\Omega}
$$

Der gesamte Kleinsignal-Eingangswiderstand ist:

$$
r_{in} =
R_{B1} \parallel R_{B2} \parallel r_{in,B}
$$

$$
\boxed{r_{in} = 21.13\,\mathrm{k\Omega}}
$$

Da $V_A = \infty$ angenommen wird, ist $r_o = \infty$.

Damit gilt näherungsweise:

$$
r_{out} = R_C \parallel r_o
$$

$$
\boxed{r_{out} \approx R_C = 1.00\,\mathrm{k\Omega}}
$$

---

## e) Kleinsignal-Spannungsverstärkung

Für die Rechnung mit Last gilt:

$$
R_C' = R_C \parallel R_L
$$

$$
R_C' = 1\,\mathrm{k\Omega} \parallel 100\,\mathrm{k\Omega}
= 990.10\,\Omega
$$

### Ohne Emitterkondensator $C_E$

Der allgemeine Ausdruck in Abhängigkeit von $g_m$ lautet:

$$
A_V =
- \frac{g_m \cdot (R_C \parallel R_L)}
{1 + g_m \cdot R_E \cdot \left(1 + \frac{1}{\beta}\right)}
$$

Da $\beta$ groß ist, kann man näherungsweise schreiben:

$$
A_V \approx
- \frac{g_m \cdot (R_C \parallel R_L)}
{1 + g_m \cdot R_E}
$$

Einsetzen mit der genauen Formel:

$$
A_V =
- \frac{0.0889\,\mathrm{S} \cdot 990.10\,\Omega}
{1 + 0.0889\,\mathrm{S} \cdot 76.74\,\Omega \cdot \left(1 + \frac{1}{294.3}\right)}
$$

$$
\boxed{A_V = -11.22}
$$

Die Schaltung invertiert das Signal, deshalb ist das Vorzeichen negativ.

### Mit sehr großem Emitterkondensator $C_E$

Wenn ein sehr großer Kondensator $C_E$ parallel zu $R_E$ geschaltet wird, wird $R_E$ für Wechselspannung praktisch kurzgeschlossen.

Dann gilt näherungsweise:

$$
A_V =
- g_m \cdot (R_C \parallel R_L)
$$

Einsetzen:

$$
A_V =
- 0.0889\,\mathrm{S} \cdot 990.10\,\Omega
$$

$$
\boxed{A_V = -88.01}
$$

Die Spannungsverstärkung wird dadurch betragsmäßig deutlich größer, aber die Gegenkopplung und damit die Linearisierung wird für Wechselspannung stark reduziert.

----

## Endergebnisse

| Aufgabe | Ergebnis |
|---|---:|
| a) $I_{C0}$ allgemein | $\frac{V_{PLUS} - V_{CE,sat}}{2R_C + \left(1 + \frac{1}{\beta}\right)R_E}$ |
| b) $I_{C0}$ | $2.311\,\mathrm{mA}$ |
| b) $V_{C0}$ | $2.689\,\mathrm{V}$ |
| b) $V_{out,max,pp}$ | $4.622\,\mathrm{V}$ |
| c) $R_{B1}$ | $329.64\,\mathrm{k\Omega}$ |
| c) $R_{B2}$ | $172.36\,\mathrm{k\Omega}$ |
| d) $r_{in}$ | $21.13\,\mathrm{k\Omega}$ |
| d) $r_{out}$ | $1.00\,\mathrm{k\Omega}$ |
| e) $A_V$ ohne $C_E$ | $-11.22$ |
| e) $A_V$ mit $C_E$ | $-88.01$ |

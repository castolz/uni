model PhysikalischesPendel
  parameter Real l = 1;          // Länge des Pendels in Metern
  parameter Real g = 9.81;       // Erdbeschleunigung in m/s^2
  parameter Real phi_0 = 1.5708; // Anfangsauslenkung (pi/4 in rad)
  parameter Real omega_0 = 0;    // Anfangsgeschwindigkeit

  Real phi(start=phi_0);         // Winkel des Pendels (in rad)
  Real omega(start=omega_0);     // Winkelgeschwindigkeit (in rad/s)

equation
  // Differentialgleichungen für das Pendel
  der(phi) = omega;
  der(omega) = -(g / l) * sin(phi);

end PhysikalischesPendel;

model pendel
  constant Real pi = Modelica.Constants.pi;
  constant Real g = 9.81;
  parameter Real l = 1.0;
  parameter Real d = 0.0;

  Real phi(start=pi/4, fixed=true);
  Real phi_dot(start=0, fixed=true);

equation
  der(phi) = phi_dot;
  der(phi_dot) + d*phi_dot + g*phi/l = 0;
end pendel;

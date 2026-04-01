model pendel
  Real phi(start=pi/4, fixed=true);
  Real phi_dot(start=0, fixed=true);
  parameter Real l=1.0;
  constant  Real g=9.81;
  constant  Real pi=Modelica.Constants.pi;
equation
  der(phi)=phi_dot;
  der(phi_dot) + g*phi/l=0;
annotation(
    uses(Modelica(version = "4.1.0")),
    experiment(StartTime = 0, StopTime = 5, Tolerance = 1e-6, Interval = 0.01));
end pendel;

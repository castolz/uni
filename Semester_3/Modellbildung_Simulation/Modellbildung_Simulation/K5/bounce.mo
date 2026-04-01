model bounce
  parameter Real alpha=0.001;
  parameter Real g=9.81;
  Real h(start=10, fixed=true) "Höhe";
  Real v(start=5, fixed=true) "Geschwindigkeit";

equation
  v=der(h);
  -g-alpha*v^3=der(v);
  when {h <= 0.0} then
    reinit(v, -pre(v));
  end when;
annotation(experiment(StartTime = 0, StopTime = 23, Tolerance = 1e-6, Interval = 0.2));
end bounce;

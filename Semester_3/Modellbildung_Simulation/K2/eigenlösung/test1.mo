model test1
  Modelica.Blocks.Continuous.Integrator integrator annotation(
    Placement(transformation(origin = {-8, 2}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Continuous.Integrator integrator1(k = 1, y_start = Modelica.Constants.pi/4)  annotation(
    Placement(transformation(origin = {26, 2}, extent = {{-10, -10}, {10, 10}})));
  Modelica.Blocks.Math.Gain gain(k = -9.81)  annotation(
    Placement(transformation(origin = {0, -40}, extent = {{10, -10}, {-10, 10}})));
equation
  connect(integrator.y, integrator1.u) annotation(
    Line(points = {{3, 2}, {14, 2}}, color = {0, 0, 127}));
  connect(integrator1.y, gain.u) annotation(
    Line(points = {{37, 2}, {37, -28}, {12, -28}, {12, -40}}, color = {0, 0, 127}));
  connect(gain.y, integrator.u) annotation(
    Line(points = {{-10, -40}, {-20, -40}, {-20, 2}}, color = {0, 0, 127}));

annotation(
    uses(Modelica(version = "4.0.0")));
end test1;

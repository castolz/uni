model pendel_block
  Modelica.Blocks.Continuous.Integrator integrator1 annotation(
    Placement(visible = true, transformation(origin = {-32, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Continuous.Integrator integrator2(y(fixed = false), y_start = Modelica.Constants.pi / 4)  annotation(
    Placement(visible = true, transformation(origin = {16, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Gain gain1(k = -9.81)  annotation(
    Placement(visible = true, transformation(origin = {-14, -56}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
equation
  connect(gain1.u, integrator2.y) annotation(
    Line(points = {{-2, -56}, {46, -56}, {46, 2}, {28, 2}, {28, 2}}, color = {0, 0, 127}));
  connect(integrator1.y, integrator2.u) annotation(
    Line(points = {{-20, 2}, {2, 2}, {2, 2}, {4, 2}}, color = {0, 0, 127}));
  connect(gain1.y, integrator1.u) annotation(
    Line(points = {{-26, -56}, {-70, -56}, {-70, 2}, {-44, 2}, {-44, 2}}, color = {0, 0, 127}));

annotation(
    uses(Modelica(version = "4.1.0")),
    experiment(StartTime = 0, StopTime = 5, Tolerance = 1e-6, Interval = 0.01));
end pendel_block;

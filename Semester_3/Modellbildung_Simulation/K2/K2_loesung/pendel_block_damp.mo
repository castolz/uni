model pendel_block_damp
  Modelica.Blocks.Continuous.Integrator integrator1 annotation(
    Placement(visible = true, transformation(origin = {-32, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Continuous.Integrator integrator2(y(fixed = false), y_start = Modelica.Constants.pi / 4)  annotation(
    Placement(visible = true, transformation(origin = {16, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Gain gain1(k = -9.81)  annotation(
    Placement(visible = true, transformation(origin = {-14, -56}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  Modelica.Blocks.Math.Gain gain2(k = -0.3) annotation(
    Placement(visible = true, transformation(origin = {-30, -28}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  Modelica.Blocks.Math.Add add1 annotation(
    Placement(visible = true, transformation(origin = {-62, 2}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
equation
  connect(gain1.y, add1.u1) annotation(
    Line(points = {{-24, -56}, {-92, -56}, {-92, 8}, {-74, 8}, {-74, 8}}, color = {0, 0, 127}));
  connect(gain2.y, add1.u2) annotation(
    Line(points = {{-40, -28}, {-82, -28}, {-82, -4}, {-74, -4}, {-74, -4}}, color = {0, 0, 127}));
  connect(gain2.u, integrator1.y) annotation(
    Line(points = {{-18, -28}, {-8, -28}, {-8, 2}, {-20, 2}, {-20, 2}}, color = {0, 0, 127}));
  connect(add1.y, integrator1.u) annotation(
    Line(points = {{-50, 2}, {-46, 2}, {-46, 2}, {-44, 2}}, color = {0, 0, 127}));
  connect(gain1.u, integrator2.y) annotation(
    Line(points = {{-2, -56}, {46, -56}, {46, 2}, {28, 2}, {28, 2}}, color = {0, 0, 127}));
  connect(integrator1.y, integrator2.u) annotation(
    Line(points = {{-20, 2}, {2, 2}, {2, 2}, {4, 2}}, color = {0, 0, 127}));

annotation(
    uses(Modelica(version = "4.1.0")),
    experiment(StartTime = 0, StopTime = 5, Tolerance = 1e-6, Interval = 0.01));end pendel_block_damp;

model pendel_block_lw
  Modelica.Blocks.Continuous.Integrator integrator1 annotation(
    Placement(visible = true, transformation(origin = {-12, 60}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Continuous.Integrator integrator2(y(fixed = false), y_start = Modelica.Constants.pi / 4)  annotation(
    Placement(visible = true, transformation(origin = {36, 60}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Gain gain1(k = -9.81)  annotation(
    Placement(visible = true, transformation(origin = {6, -40}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  Modelica.Blocks.Math.Gain gain2(k = -1.63e-4)  annotation(
    Placement(visible = true, transformation(origin = {-30, -20}, extent = {{-10, -10}, {10, 10}}, rotation = 180)));
  Modelica.Blocks.Math.Add add1 annotation(
    Placement(visible = true, transformation(origin = {-42, 60}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Abs abs1 annotation(
    Placement(visible = true, transformation(origin = {4, 26}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Math.Product product1 annotation(
    Placement(visible = true, transformation(origin = {10, -4}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Blocks.Math.Sin sin1 annotation(
    Placement(visible = true, transformation(origin = {60, 8}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
equation
  connect(sin1.y, gain1.u) annotation(
    Line(points = {{60, -4}, {60, -4}, {60, -40}, {18, -40}, {18, -40}}, color = {0, 0, 127}));
  connect(integrator2.y, sin1.u) annotation(
    Line(points = {{48, 60}, {60, 60}, {60, 20}, {60, 20}, {60, 20}}, color = {0, 0, 127}));
  connect(product1.y, gain2.u) annotation(
    Line(points = {{10, -16}, {10, -16}, {10, -20}, {-18, -20}, {-18, -20}}, color = {0, 0, 127}));
  connect(product1.u1, integrator2.u) annotation(
    Line(points = {{16, 8}, {16, 8}, {16, 60}, {24, 60}, {24, 60}}, color = {0, 0, 127}));
  connect(product1.u2, abs1.y) annotation(
    Line(points = {{4, 8}, {4, 8}, {4, 14}, {4, 14}}, color = {0, 0, 127}));
  connect(abs1.u, integrator1.y) annotation(
    Line(points = {{4, 38}, {4, 38}, {4, 60}, {0, 60}, {0, 60}}, color = {0, 0, 127}));
  connect(integrator1.y, integrator2.u) annotation(
    Line(points = {{-1, 60}, {21, 60}, {21, 60}, {23, 60}}, color = {0, 0, 127}));
  connect(add1.y, integrator1.u) annotation(
    Line(points = {{-31, 60}, {-27, 60}, {-27, 60}, {-25, 60}}, color = {0, 0, 127}));
  connect(gain2.y, add1.u2) annotation(
    Line(points = {{-41, -20}, {-63, -20}, {-63, 54}, {-54, 54}}, color = {0, 0, 127}));
  connect(gain1.y, add1.u1) annotation(
    Line(points = {{-5, -40}, {-73, -40}, {-73, 66}, {-54, 66}}, color = {0, 0, 127}));

annotation(
    uses(Modelica(version = "4.1.0")),
    experiment(StartTime = 0, StopTime = 500, Tolerance = 1e-6, Interval = 0.1));
end pendel_block_lw;

model A3
  import SI = Modelica.Units.SI;
  parameter SI.Time T_p = 5e-3;
  parameter SI.Frequency f = 50;
  parameter SI.Voltage u_dach = 200;
  Modelica.Electrical.Analog.Basic.Inductor inductor1(L = 10e-3, i(fixed = true, start = 30)) annotation(
    Placement(visible = true, transformation(origin = {66, 30}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  Modelica.Electrical.Analog.Basic.Resistor resistor1(R = 10) annotation(
    Placement(visible = true, transformation(origin = {24, 40}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Electrical.Analog.Basic.Ground ground1 annotation(
    Placement(visible = true, transformation(origin = {-80, -12}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Electrical.Analog.Sources.ConstantVoltage constantVoltage1(V = 300) annotation(
    Placement(visible = true, transformation(origin = {-80, 30}, extent = {{-10, -10}, {10, 10}}, rotation = -90)));
  ModSimBib.vierqst vierqst1(T_p = 5e-3) annotation(
    Placement(visible = true, transformation(origin = {-4, 30}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.Step step1(height = 60, offset = 90, startTime = 5e-3 - 1e-6)  annotation(
    Placement(visible = true, transformation(origin = {-80, -36}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Sources.Step step2(height = 60, startTime = 10e-3 - 1e-6)  annotation(
    Placement(visible = true, transformation(origin = {-70, -66}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
  Modelica.Blocks.Math.Add add1 annotation(
    Placement(visible = true, transformation(origin = {-34, -18}, extent = {{-10, -10}, {10, 10}}, rotation = 0)));
equation
  connect(step2.y, add1.u2) annotation(
    Line(points = {{-58, -66}, {-52, -66}, {-52, -24}, {-46, -24}, {-46, -24}}, color = {0, 0, 127}));
  connect(step1.y, add1.u1) annotation(
    Line(points = {{-68, -36}, {-60, -36}, {-60, -12}, {-46, -12}, {-46, -12}}, color = {0, 0, 127}));
  connect(add1.y, vierqst1.u_e) annotation(
    Line(points = {{-22, -18}, {-20, -18}, {-20, 30}, {-16, 30}, {-16, 30}}, color = {0, 0, 127}));
  connect(vierqst1.p1, constantVoltage1.p) annotation(
    Line(points = {{-14, 40}, {-80, 40}}, color = {0, 0, 255}));
  connect(vierqst1.p2, resistor1.p) annotation(
    Line(points = {{6, 40}, {14, 40}}, color = {0, 0, 255}));
  connect(vierqst1.n1, constantVoltage1.n) annotation(
    Line(points = {{-14, 20}, {-80, 20}}, color = {0, 0, 255}));
  connect(vierqst1.n2, inductor1.n) annotation(
    Line(points = {{6, 20}, {66, 20}}, color = {0, 0, 255}));
  connect(ground1.p, constantVoltage1.n) annotation(
    Line(points = {{-80, -2}, {-80, 20}}, color = {0, 0, 255}));
  connect(resistor1.n, inductor1.p) annotation(
    Line(points = {{34, 40}, {66, 40}, {66, 40}, {66, 40}}, color = {0, 0, 255}));
  annotation(
    uses(Modelica(version = "4.1.0")),
    experiment(StartTime = 0, StopTime = 15e-3, Tolerance = 1e-06, Interval = 3.75e-05),
    Diagram(coordinateSystem(initialScale = 0.1)));
end A3;

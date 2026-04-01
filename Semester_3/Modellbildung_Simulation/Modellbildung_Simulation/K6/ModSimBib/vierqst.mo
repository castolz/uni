within ModSimBib;
model vierqst
  parameter Real T_p = 1e-3;  // Pulsdauer
  constant Real pi=Modelica.Constants.pi;
  discrete Real u_hold(start=0, fixed=true), u_zk(start=0, fixed=true);
  Real u_out;  // Hilfsvariable zum Plotten
Modelica.Blocks.Interfaces.RealInput u_e "Eingangsspannung" annotation (Placement(
        visible = true, transformation(origin = {-120, -2},extent = {{-20, -20}, {20, 20}}, rotation = 0), iconTransformation(origin = {-120, -2},extent = {{-20, -20}, {20, 20}}, rotation = 0)));
  Real dir(start=-1, fixed=true), cnt(start=0, fixed=true);
  Modelica.Electrical.Analog.Interfaces.PositivePin p1 annotation(
    Placement(transformation(extent = {{-110, 90}, {-90, 110}}), iconTransformation(extent = {{-110, 90}, {-90, 110}})));
  Modelica.Electrical.Analog.Interfaces.NegativePin n1 annotation(
    Placement(transformation(extent = {{-90, -110}, {-110, -90}}), iconTransformation(extent = {{-90, -110}, {-110, -90}})));
  Modelica.Electrical.Analog.Interfaces.PositivePin p2 annotation(
    Placement(transformation(extent = {{110, 90}, {90, 110}}), iconTransformation(extent = {{110, 90}, {90, 110}})));
  Modelica.Electrical.Analog.Interfaces.NegativePin n2 annotation(
    Placement(transformation(extent = {{90, -110}, {110, -90}}), iconTransformation(extent = {{90, -110}, {110, -90}})));
equation
  when sample(0,T_p/2) then
    dir=-pre(dir);
  end when;
when sample(0, T_p) then
    u_hold = u_e;  // Sample (Hold durch discrete)
    u_zk = p1.v - n1.v;
end when;
  der(cnt)=dir*2/T_p;
  u_out = if cnt < 1 - abs(u_hold) / u_zk then 0 else u_zk * sign(u_hold);
  if cnt < 1 - abs(u_hold) / u_zk then
    // Kurzschluß und Leerlauf (mit Bezug zu Ground)
    p1.i = 0; p2.v - n2.v = 0; n2.v - n1.v = 0; 0 = p2.i + n2.i + n1.i;
  elseif u_hold > 0 then
    // Direkt verbunden
    p2.v = p1.v; n2.v = n1.v; p1.i + p2.i = 0; n1.i + n2.i = 0;
  else
    // über Kreuz verbunden
    p2.v = n1.v; n2.v = p1.v; p1.i + n2.i = 0; n1.i + p2.i = 0;
  end if;
end vierqst;
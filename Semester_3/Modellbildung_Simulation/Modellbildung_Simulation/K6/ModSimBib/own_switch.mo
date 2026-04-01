within ModSimBib;
model own_switch
  extends Modelica.Electrical.Analog.Interfaces.OnePort;
  Modelica.Blocks.Interfaces.BooleanInput control
    "true => switch open, false => p--n connected" annotation (Placement(
        transformation(
        origin={0,70},
        extent={{-20,-20},{20,20}},
        rotation=270)));
protected
  Real s(start=1);
equation
  v = s * (if control then 1 else 0);
  i = s * (if control then 0 else 1);
end own_switch;

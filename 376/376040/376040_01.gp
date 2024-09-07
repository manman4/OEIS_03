\\ E.g.f.: Series_Reversion( (1 - x)^3 * (1 - exp(-x)) ).
my(N=20, x='x+O('x^N)); concat(0, Vec(serlaplace(serreverse( (1 - x)^3 * (1 - exp(-x)) ))))

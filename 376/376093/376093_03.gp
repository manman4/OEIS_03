\\ E.g.f.: Series_Reversion( x * exp(-2*x) / (1 + x) ).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(serreverse( x * exp(-2*x) / (1 + x) ))))




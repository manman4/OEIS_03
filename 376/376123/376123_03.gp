\\ E.g.f.: Series_Reversion( x * exp(-x) / (1 + 2*x) ).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(serreverse( x * exp(-x) / (1 + 2*x) ))))



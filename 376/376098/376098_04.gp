\\ E.g.f.: Series_Reversion( x / (x + exp(2*x)) ).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(serreverse( x / (x + exp(2*x)) ))))



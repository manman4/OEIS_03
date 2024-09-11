\\ E.g.f.: Series_Reversion( x / (3*x + exp(-x)) ).
my(N=30, x='x+O('x^N)); concat(0, Vec(serlaplace(serreverse( x / (3*x + exp(-x)) ))))


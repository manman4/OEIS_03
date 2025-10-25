\\ E.g.f.: x / Series_Reversion( x * exp(x/(1-x)) ). 
my(N=30, x='x+O('x^N)); Vec(serlaplace(x / serreverse( x * exp(x/(1-x)) )))  
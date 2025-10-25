\\ E.g.f.: exp( Series_Reversion( x*exp(3*x)/(1+x) ) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace( exp(serreverse( x * exp(3*x)/(1+x) ) )))    
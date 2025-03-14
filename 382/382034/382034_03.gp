\\ E.g.f.: A(x) = exp( Series_Reversion( x/(1 + x*exp(x))^4 ) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( serreverse(x/(1 + x*exp(x))^4) ) ))


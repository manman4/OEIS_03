\\ E.g.f.: 1 + log( (1/x) * Series_Reversion( x*exp(-x*(2+x)) ) ).
my(N=20, x='x+O('x^N)); Vec(serlaplace(1+log((1/x)*serreverse(x*exp(-x*(2+x))))))


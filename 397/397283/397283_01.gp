\\ Expansion of e.g.f. (1/x) * Series_Reversion( x + log(1-x^4) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(serreverse(x+log(1-x^4))/x))
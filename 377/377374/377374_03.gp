\\ Expansion of e.g.f. (1/x) * Series_Reversion( x / (exp(-x) + 3*x) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(serreverse(x/(exp(-x)+3*x))/x))



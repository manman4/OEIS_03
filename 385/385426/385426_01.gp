\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-arcsin(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(serreverse(x*exp(-asin(x)))/x))


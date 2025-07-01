\\ Expansion of e.g.f. (1/x) * Series_Reversion( x * exp(-arctanh(x)) ).
my(N=30, x='x+O('x^N)); Vec(serlaplace(serreverse(x*exp(-atanh(x)))/x))


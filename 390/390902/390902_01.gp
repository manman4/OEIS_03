\\ Expansion of e.g.f. Series_Reversion(4*x + log(1-3*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(4*x+log(1-3*x))))


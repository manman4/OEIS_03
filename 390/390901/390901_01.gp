\\ Expansion of e.g.f. Series_Reversion(3*x + log(1-2*x)).
my(N=20, x='x+O('x^N)); Vec(serlaplace(serreverse(3*x+log(1-2*x))))


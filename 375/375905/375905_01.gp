\\ E.g.f.: A(x) = ( (1/x) * Series_Reversion(x / (1 - log(1-x))) )^3.
my(x='x+O(x^20)); Vec(serlaplace((serreverse(x / (1 - log(1-x))) / x)^3))

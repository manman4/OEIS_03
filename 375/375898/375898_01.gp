\\ E.g.f.: A(x) = ( (1/x) * Series_Reversion(x * (2 - exp(x))) )^3.
my(x='x+O(x^20)); Vec(serlaplace((serreverse(x * (2 - exp(x))) / x)^3))


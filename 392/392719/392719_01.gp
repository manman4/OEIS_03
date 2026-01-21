M=40;

\\ Expansion of e.g.f. Series_Reversion(1 - exp(-x) - x^3).
my(N=M, x='x+O('x^N)); Vec(serlaplace(serreverse(1 - exp(-x) - x^3)))
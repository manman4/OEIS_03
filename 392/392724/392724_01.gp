M=40;

\\ Expansion of e.g.f. Series_Reversion(x - (exp(x) - 1)^4).
my(N=M, x='x+O('x^N)); Vec(serlaplace(serreverse(x - (exp(x) - 1)^4)))
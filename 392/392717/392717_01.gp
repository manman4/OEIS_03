M=40;

\\ Expansion of e.g.f. Series_Reversion(exp(x) - 1 - x^4).
my(N=M, x='x+O('x^N)); Vec(serlaplace(serreverse(exp(x) - 1 - x^4)))
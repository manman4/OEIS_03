M=40;

\\ Expansion of e.g.f. Series_Reversion(log(1+x) - x^4).
my(N=M, x='x+O('x^N)); Vec(serlaplace(serreverse(log(1+x) - x^4)))
M=30;

\\ Expansion of e.g.f. 1/(1 + x*log(1-x)^3).
my(N=M, x='x+O('x^N)); Vec(serlaplace( 1/(1 + x*log(1 - x)^3) ))

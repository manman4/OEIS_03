M=30;

\\ Expansion of e.g.f. 1/(1 - x*(exp(x) - 1)^2).
my(N=M, x='x+O('x^N)); Vec(serlaplace( 1/(1 - x*(exp(x) - 1)^2) ))
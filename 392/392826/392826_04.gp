M=40;

\\ E.g.f.: 2/(1 + sqrt(1-4*x*(exp(x) - 1)^2)).
my(N=M, x='x+O('x^N)); Vec(serlaplace( 2/(1 + sqrt(1-4*x*(exp(x) - 1)^2)) ))
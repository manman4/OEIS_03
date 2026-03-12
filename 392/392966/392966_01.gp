M=30;

\\ E.g.f.: 2 / (2+x-exp(x) + sqrt((2+x-exp(x))^2 - 4*x)).
my(N=M, x='x+O('x^N)); Vec(serlaplace( 2 / (2+x-exp(x) + sqrt((2+x-exp(x))^2 - 4*x)) ) )

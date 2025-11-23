M=15;

\\ E.g.f.: exp( 2*(exp(3*x)-1)/3 ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( exp( 2*(exp(3*x)-1)/3 ) ))
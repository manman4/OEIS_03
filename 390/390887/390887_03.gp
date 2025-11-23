M=15;

\\ E.g.f.: exp( 3*(exp(2*x)-1)/2 ).
my(N=M, x='x+O('x^N)); Vec(serlaplace( exp( 3*(exp(2*x)-1)/2 ) ))
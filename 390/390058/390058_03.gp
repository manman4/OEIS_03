\\ E.g.f.: exp( -LambertW(-2*x * (1+x^2))/2 ).
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-2*x * (1+x^2))/2 ) ))

 
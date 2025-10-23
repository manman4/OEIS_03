\\ E.g.f.: exp(B(x)), where B(x) is the e.g.f. of A376104.
\\ B = -LambertW(-2*x / (1 + x))/2.
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-2*x / (1 + x))/2 ) ))

 
\\ E.g.f.: exp(B(x)), where B(x) is the e.g.f. of A376105.
\\ B = -LambertW(-3*x / (1 + x))/3.
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-3*x / (1 + x))/3 ) ))

 
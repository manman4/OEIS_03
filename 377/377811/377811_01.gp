\\ E.g.f.: exp( -LambertW(-x/(1-x)^3) )/(1-x)^3.
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x/(1-x)^3) )/(1-x)^3 ))

\\ E.g.f.: -LambertW(-x/(1-x)^3)/x.
my(N=20, x='x+O('x^N)); Vec(serlaplace( -lambertw(-x/(1-x)^3)/x ))


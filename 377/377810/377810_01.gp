\\ E.g.f.: exp( -LambertW(-x/(1-x)^2) )/(1-x)^2.
my(N=20, x='x+O('x^N)); Vec(serlaplace( exp( -lambertw(-x/(1-x)^2) )/(1-x)^2 ))

\\ E.g.f.: -LambertW(-x/(1-x)^2)/x.
my(N=20, x='x+O('x^N)); Vec(serlaplace( -lambertw(-x/(1-x)^2)/x ))

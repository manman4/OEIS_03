M=15;

\\ E.g.f.: 1 / (2 * (1 + LambertW( -(1/2) * exp(x-1/2) ))).
my(N=M, x='x+O('x^N)); Vec(serlaplace( 1 / (2 * (1 + lambertw( -(1/2) * exp(x-1/2)) )) ))
my(N=M, x='x+O('x^N)); apply(round, Vec(serlaplace( 1 / (2 * (1 + lambertw( -(1/2) * exp(x-1/2)) )) )))
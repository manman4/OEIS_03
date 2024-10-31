\\ Expansion of e.g.f. 1/(1 - x * exp(x))^2.
my(N=25, x='x+O('x^N)); Vec(serlaplace( 1/(1 - x * exp(x))^2 ))



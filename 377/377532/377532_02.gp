\\ Expansion of e.g.f. 1/(1 - x^2 * exp(x))^3.
my(N=25, x='x+O('x^N)); Vec(serlaplace( 1/(1 - x^2 * exp(x))^3 ))


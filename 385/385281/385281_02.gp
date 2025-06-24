\\ Expansion of e.g.f. 1/(1 - 2 * x * cosh(2*x))^(1/2).
my(N=25, x='x+O('x^N)); Vec(serlaplace( 1/(1 - 2 * x * cosh(2*x))^(1/2) ))


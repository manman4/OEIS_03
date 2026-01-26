\\ Expansion of e.g.f. 1/(1 + LambertW(-x^3 * (exp(x) - 1))).
my(N=30, x='x+O('x^N)); Vec(serlaplace( 1/(1 + lambertw(-x^3 * (exp(x) - 1))) ))

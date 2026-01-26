\\ Expansion of e.g.f. 1/(1 + LambertW(x^3 * log(1-x))).
my(N=30, x='x+O('x^N)); Vec(serlaplace( 1/(1 + lambertw(x^3 * log(1-x))) ))

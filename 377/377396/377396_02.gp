\\ Expansion of e.g.f. (1 + log(1+x))^3.
my(N=40, x='x+O('x^N)); Vec(serlaplace( (1 + log(1+x))^3 ))

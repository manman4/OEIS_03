\\ Expansion of e.g.f. -log(1 - (exp(2*x) - 1)/2)^3 / 6.
my(N=30, x='x+O('x^N)); concat([0, 0, 0], Vec(serlaplace( -log(1 - (exp(2*x) - 1)/2)^3 / 6 )))  

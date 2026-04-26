\\ E.g.f.: 8/27 * exp(3*x) - (1+2*x)/4 * exp(2*x) - 5/108 + x/9 + x^2/6.
my(N=30, x='x+O('x^N)); concat([0, 0, 0, 0], Vec(serlaplace( 8/27 * exp(3*x) - (1+2*x)/4 * exp(2*x) - 5/108 + x/9 + x^2/6 )))
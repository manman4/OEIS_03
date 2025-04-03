\\ Expansion of e.g.f. exp( x/(1-3*x)^(4/3) ).
my(M=30, x='x+O('x^M)); Vec(serlaplace( exp( x/(1-3*x)^(4/3) ) ))



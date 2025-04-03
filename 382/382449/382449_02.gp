\\ Expansion of e.g.f. exp( x/(1-2*x)^(3/2) ).
my(M=30, x='x+O('x^M)); Vec(serlaplace( exp( x/(1-2*x)^(3/2) ) ))




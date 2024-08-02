\\ exp( (1/(1 - 4*x)^(1/2) - 1)/2 ).

my(N=20, x='x+O('x^N)); Vec(serlaplace(exp( (1/(1 - 4*x)^(1/2) - 1)/2)))
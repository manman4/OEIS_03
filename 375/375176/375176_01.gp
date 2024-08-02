\\ exp( (exp( (exp(9*x) - 1)/3 ) - 1)/3 ).

my(N=20, x='x+O('x^N)); Vec(serlaplace(exp( (exp( (exp(9*x) - 1)/3 ) - 1)/3 )))

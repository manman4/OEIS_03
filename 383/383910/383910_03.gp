\\ Expansion of Product_{k=0..3} (1 + k*x)/(1 - k*x).
my(N=40, x='x+O('x^N)); Vec(prod(k=0, 3, (1+k*x)/(1-k*x)))



\\ Expansion of Product_{k>=1} 1/(1 - (2^k - 1) * x^k).
my(N=20, x='x+O('x^N)); Vec(prod(k=1, N, 1/(1 - (2^k - 1) * x^k)))


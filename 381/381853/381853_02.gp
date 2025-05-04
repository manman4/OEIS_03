\\ Expansion of 1/( Product_{k=0..3} (1 + (-1)^k * (2*k+1) * x) ).
my(N=20, x='x+O('x^N)); Vec(1/prod(k=0, 3, (1+(-1)^k*(2*k+1)*x)))


\\ Expansion of 1/Product_{k=0..3} (1 - 3^k * 4^(3-k) * x).
my(N=30, x='x+O('x^N)); Vec( 1/prod(k=0, 3, (1 - 3^k * 4^(3-k) * x)) )

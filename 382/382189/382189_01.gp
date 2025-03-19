\\ Expansion of 1/(1 - 4 * Sum_{k>=0} x^(3^k))^(1/2).
my(N=30, x='x+O('x^N)); Vec( 1/(1 - 4 * sum(k=0, 8, x^(3^k)) )^(1/2) )


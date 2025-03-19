\\ Expansion of 1/(1 - 9 * Sum_{k>=0} x^(2^k))^(1/3).
my(N=30, x='x+O('x^N)); Vec( 1/(1 - 9 * sum(k=0, 9, x^(2^k)) )^(1/3) )


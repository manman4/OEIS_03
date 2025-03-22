\\ Expansion of ( 1 + 4 * Sum_{k>=0} x^(2^k)/(1 - x^(2^k)) )^(1/2).
my(N=50, x='x+O('x^N)); Vec( ( 1 + 4 * sum(k=0, 8, x^(2^k) / (1 - x^(2^k)) ) )^(1/2) )

\\ Expansion of ( 1 + 9 * Sum_{k>=0} x^(2^k)/(1 - x^(2^k)) )^(1/3).
my(N=50, x='x+O('x^N)); Vec( ( 1 + 9 * sum(k=0, 8, x^(2^k) / (1 - x^(2^k)) ) )^(1/3) )  

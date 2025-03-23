\\ Expansion of 1/( 1 - Sum_{k>=0} x^(5^k) / (1 - x^(5^k)) ).
my(N=50, x='x+O('x^N)); Vec( 1/( 1 - sum(k=0, 6, x^(5^k) / (1 - x^(5^k)) ) ) )

\\ Expansion of 1/( 1 - Sum_{k>=0} x^(3^k) / (1 - x^(3^k)) ).
my(N=50, x='x+O('x^N)); Vec( 1/( 1 - sum(k=0, 8, x^(3^k) / (1 - x^(3^k)) ) ) )  

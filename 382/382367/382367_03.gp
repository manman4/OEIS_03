\\ G.f.: 1/(1 - Sum_{i>=1, j>=0} x^(i*3^j)).
my(N=100, x='x+O('x^N)); Vec( 1/(1 - sum(i=1, N, sum(j=0, 5, x^(i*3^j) )) ) )

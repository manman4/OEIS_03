\\ 1 + Sum_{i>=1} Sum_{j>=1} x^(i*j) * Product_{k=1..i*j-1} (1+x^k).
my(N=66, x='x+O('x^N)); Vec(1+sum(i=1, N, sum(j=1, (N+1)\i, x^(i*j)*prod(k=1, i*j-1, 1+x^k))))

my(N=50, x='x+O('x^N)); Vec(1/prod(k=1, N, (1-x^k)^(valuation(k, 5)+1)))

\\ Let A(x) be the g.f. of this sequence, and P(x) be the g.f. of A000041, then P(x) = A(x)/A(x^5).
my(N=60, x='x+O('x^N)); Vec(prod(k=1, N, ( (1-x^(5*k)) / (1-x^k) )^(valuation(k, 5)+1)))


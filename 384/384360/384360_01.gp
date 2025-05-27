\\ G.f.: exp((1/27) * Sum_{k>=1} A384364(3,k) * x^k/k).
a384364(n, k) = sum(i=0, k*n, 3^i*sum(j=0, i, (-1)^j*binomial(i, j)*binomial(i-j, n)^k));
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a384364(3, k)*x^k/k)/27))
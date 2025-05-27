\\ G.f.: exp((1/4) * Sum_{k>=1} A384362(2,k) * x^k/k).
a384362(n, k) = sum(i=0, k*n, 2^i*sum(j=0, i, (-1)^j*binomial(i, j)*binomial(i-j, n)^k));
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a384362(2, k)*x^k/k)/4))
\\ G.f.: exp(Sum_{k>=1} A062208(k) * x^k/k).
a262809(n, k) = sum(i=0, k*n, sum(j=0, i, (-1)^j*binomial(i, j)*binomial(i-j, n)^k));
a062208(n) = a262809(3, n);
for(n=0, 20, print1(a062208(n), ", "));
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a062208(k)*x^k/k)))
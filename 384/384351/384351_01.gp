\\ G.f.: exp(Sum_{k>=1} A055203(k) * x^k/k).
a262809(n, k) = sum(i=0, k*n, sum(j=0, i, (-1)^j*binomial(i, j)*binomial(i-j, n)^k));
a055203(n) = a262809(2, n);
for(n=0, 20, print1(a055203(n), ", "));
my(N=20, x='x+O('x^N)); Vec(exp(sum(k=1, N, a055203(k)*x^k/k)))
M=31;

a(n) = binomial(n+4,4) + sum(k=0, n-5, a(k) * a(n-k-5));
for(n=0, M, print1(a(n), ", "));
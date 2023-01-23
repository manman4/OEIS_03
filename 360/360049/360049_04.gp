M=33;

a(n) = binomial(n+2,2) - sum(k=0, n-3, a(k) * a(n-k-3));
for(n=0, M, print1(a(n), ", "));
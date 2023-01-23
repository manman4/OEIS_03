M=32;

a(n) = binomial(n+3,3) - sum(k=0, n-4, a(k) * a(n-k-4));
for(n=0, M, print1(a(n), ", "));
M=15;

a(n) = binomial(n+3,3) - sum(k=0, n-1, a(k) * a(n-k-1));
for(n=0, M, print1(a(n), ", "));
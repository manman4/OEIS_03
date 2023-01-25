M=26;

a(n) = sum(k=0, n, binomial(5*k, n-k));
for(n=0, M, print1(a(n), ", "));
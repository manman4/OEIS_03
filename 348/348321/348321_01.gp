M=14;

a(n) = sum(k=0, n, (-1)^k*binomial(n*(n-k), n*k));
for(n=0, M, print1(a(n), ", "));
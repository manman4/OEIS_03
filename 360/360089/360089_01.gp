M=31;

a(n) = sum(k=0, n, (-1)^k*binomial(5*k, n-k));
for(n=0, M, print1(a(n), ", "));
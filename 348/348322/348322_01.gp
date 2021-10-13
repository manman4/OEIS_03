M=12;

a(n) = sum(k=0, n, (-1)^k*binomial(n^2-k, n*k));
for(n=0, M, print1(a(n), ", "));
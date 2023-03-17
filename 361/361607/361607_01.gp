M=15;

a(n) = sum(k=0, n, (n*k+n-k)!/(n*k)!*binomial(n, k));
for(n=0, M, print1(a(n), ", "));

M=34;

a(n) = sum(k=1, n, k^4*binomial(n\k+1, 2));
for(n=1, M, print1(a(n), ", "));
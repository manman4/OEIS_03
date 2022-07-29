M=39;

a(n) = sum(k=1, n, k^3*binomial(n\k+1, 2));
for(n=1, M, print1(a(n), ", "));
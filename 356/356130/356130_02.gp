M=19;

a(n) = sum(k=1, n, k^(n-1)*(n\k));
for(n=1, M, print1(a(n), ", "));
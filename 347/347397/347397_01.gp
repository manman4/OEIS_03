M=66;

a(n) = sum(k=1, n, k^k*(n\k^k));
for(n=1, M, print1(a(n), ", "));
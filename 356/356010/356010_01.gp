M=21;

a(n) = n!*sum(k=1, n, n\k/k);
for(n=1, M, print1(a(n), ", "));
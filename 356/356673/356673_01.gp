M=14;

a(n) = n!*sum(k=0, n, k^(3*(n-k))/k!);
for(n=0, M, print1(a(n), ", "));
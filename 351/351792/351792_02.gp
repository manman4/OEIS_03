M=19;

a(n) = n!*sum(k=0, n, (-3*(n-k))^k/k!);
for(n=0, M, print1(a(n), ", "));
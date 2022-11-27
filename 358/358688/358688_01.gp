M=13;

a(n) = n!*sum(k=0, n, k^(k*(n-k))/(n-k)!);
for(n=0, M, print1(a(n), ", "));

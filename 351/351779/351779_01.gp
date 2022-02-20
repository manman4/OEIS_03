M=15;

a(n) = n!*sum(k=0, n, (-n)^(n-k)*(n-k)^k/k!);
for(n=0, M, print1(a(n), ", "));
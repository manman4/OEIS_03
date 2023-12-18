a(n) = n!*sum(k=0, n, 2^(n-k)*(n-k+1)^k/k!);
for(n=0, 18, print1(a(n), ", "));


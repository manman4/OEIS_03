M=14;

a(n) = sum(k=0, n, (-1)^(n-k)*k^(k*(n-k)));
for(n=0, M, print1(a(n), ", "));
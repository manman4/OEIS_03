M=8;

a(n) = sum(k=0, n, (-1)^(n-k)*k^(k*n));
for(n=0, M, print1(a(n), ", "));
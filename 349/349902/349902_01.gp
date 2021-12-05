M=12;

a(n) = sum(k=0, n, (-1)^(n-k)*k^(3*n));
for(n=0, M, print1(a(n), ", "));
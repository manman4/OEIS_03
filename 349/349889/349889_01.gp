M=13;

a(n) = sum(k=0, n, (-1)^(n-k)*k^(2*n));
for(n=0, M, print1(a(n), ", "));
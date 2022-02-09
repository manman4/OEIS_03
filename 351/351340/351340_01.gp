M=18;

a(n) = sum(k=0, n, n^k*k^(n-k));
for(n=0, M, print1(a(n), ", "));
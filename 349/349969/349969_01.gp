M=18;

a(n) = sum(k=0, n, (k*n)^(n-k));
for(n=0, M, print1(a(n), ", "));
M=16;

a(n) = sum(k=0, n, k^(n-k) * (3*k+1)^(k-1) * binomial(n,k));
for(n=0, M, print1(a(n), ", "))

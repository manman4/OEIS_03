M=18;

a(n) = sum(k=0, (n-1)\2, n^(n-1-k)*binomial(n, 2*k+1));
for(n=0, M, print1(a(n), ", "));



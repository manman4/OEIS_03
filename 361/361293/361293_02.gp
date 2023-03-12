M=19;

a(n) = sum(k=0, (n-1)\2, 10^(n-1-k)*binomial(n, 2*k+1));
for(n=0, M, print1(a(n), ", "));


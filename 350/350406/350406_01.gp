M=28;

a(n) = (-1)^n*sum(k=0, n, binomial(n-1+k, k)*binomial(n, 4*k));
for(n=0, M, print1(a(n), ", "));
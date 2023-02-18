M=18;

a(n) = sum(k=0, n\4, (n-3*k)^(n-2*k)*binomial(n-3*k, k));
for(n=0, M, print1(a(n), ", "));
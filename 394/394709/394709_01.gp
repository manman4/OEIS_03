a(n) = sum(k=0, n, binomial(n, k)*binomial(3*n, n+2*k));
for(n=0, 30, print1(a(n),", "));

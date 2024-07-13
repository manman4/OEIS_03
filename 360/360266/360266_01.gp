M=25;

a(n) = sum(k=0, n\3, binomial(n-2*k, k)*binomial(2*(n-2*k), n-2*k));
for(n=0, M, print1(a(n), ", "));
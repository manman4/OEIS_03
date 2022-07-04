M=12;

a(n) = sum(k=0, n, k^(3*k)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));
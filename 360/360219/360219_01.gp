M=26;

a(n) = sum(k=0, n\3, (-1)^k*binomial(n-3*k, k)*binomial(2*(n-3*k), n-3*k));
for(n=0, M, print1(a(n), ", "));
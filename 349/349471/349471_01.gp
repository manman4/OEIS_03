M=17;

a(n) = sum(k=0, n, (-1)^(n-k)*binomial(k*n, k));
for(n=0, M, print1(a(n), ", "));
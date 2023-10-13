M=20;

a(n) = sum(k=0, n, k!^(k+1)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));
M=30;

a(n) = sum(k=0, n-1, k!*binomial(n-1, k)*binomial(2*n-1, k));
for(n=1, M, print1(a(n), ", "));
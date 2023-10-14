M=30;

a(n) = numerator(sum(j=0, n, (-1)^(n+j-1)*binomial(n, j)*sum(k=0, n+j-1, (-1)^k/k!)));
for(n=1, M, print1(a(n), ", "));
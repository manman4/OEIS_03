M=17;

a(n) = n!*sum(k=0, n, (n+k+1)^(k-1)*binomial(n-1, n-k)/k!);
for(n=0, M, print1(a(n),", "))

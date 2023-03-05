M=17;

a(n) = (-1)^n*n!*sum(k=0, n, (-2)^k*(k+1)^(k-1)*binomial(n-1, n-k)/k!);
for(n=0, M, print1(a(n),", "))

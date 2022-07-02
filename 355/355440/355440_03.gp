M=14;

a(n) = sum(k=0, n, 4^(k*(n-k))*binomial(n, k));
for(n=0, M, print1(a(n),", "))  
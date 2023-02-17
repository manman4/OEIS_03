M=48;

a(n) = sum(k=0, n\2, (-1)^(k*(n-k))*binomial(n-k, k));
for(n=0, M, print1(a(n), ", "));
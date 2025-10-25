M=20;

a(n) = if(n==0, 1, n!*sum(k=1, n, (-n+1)^(k-1)*binomial(n-1, n-k)/k!));
for(n=0, M, print1(a(n),", "));

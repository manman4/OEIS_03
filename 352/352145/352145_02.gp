M=25;

a(n) = if(n==0, 1, sum(k=1, n, (-1)^(k\2)*binomial(n-1, k-1)*a(n-k)));
for(n=0, M, print1(a(n), ", "));
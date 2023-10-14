M=18;

a(n) = if(n==0, 1, sum(k=1, n, ((-1)^k*(k-1)!-1)*binomial(n, k)*a(n-k)));
for(n=0, M, print1(a(n), ", "));
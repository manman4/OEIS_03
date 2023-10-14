M=23;

a(n) = if(n==0, 1, sum(k=0, (n-1)\2, (-1)^k*(k+1)*binomial(n, 2*k+1)*a(n-2*k-1)));
for(n=0, M, print1(a(n), ", "));
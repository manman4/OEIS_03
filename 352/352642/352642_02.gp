M=22;

a(n) = if(n==0, 1, 2*sum(k=0, (n-1)\2, (-1)^k*(2*k+1)*binomial(n-1, 2*k)*a(n-2*k-1)));
for(n=0, M, print1(a(n), ", "));
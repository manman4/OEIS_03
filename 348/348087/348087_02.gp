M=16;

a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k*(2*k+1)^(2*n-1)*binomial(n-1, k))/((-2)^(n-1)*(n-1)!));
for(n=0, M, print1(a(n), ", "));
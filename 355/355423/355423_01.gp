M=9;

T(n, k) = if(n==0,1, sum(i=1, n, (sum(j=1, k, j^i)) * binomial(n-1, i-1) * T(n-i, k)));
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")))  
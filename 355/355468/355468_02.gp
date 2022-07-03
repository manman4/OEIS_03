M=13;

a(n) = if(n==0, 1, sum(k=1, n, k^(2*n)*binomial(n-1, k-1)));
for(n=0, M, print1(a(n), ", "));
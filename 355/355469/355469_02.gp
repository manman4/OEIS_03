M=12;

a(n) = if(n==0, 1, sum(k=1, n, k^(3*n)*binomial(n-1, k-1)));
for(n=0, M, print1(a(n), ", "));
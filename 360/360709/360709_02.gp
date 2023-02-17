M=29;

a(n) = if(n==0, 1, sum(k=1, n\3, k^(n-3*k)*binomial(n-2*k-1, k-1)));
for(n=0, M, print1(a(n), ", "));
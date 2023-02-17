M=27;

a(n) = if(n==0, 1, sum(k=1, n\2, k^(n-2*k)*binomial(n-k-1, k-1)));
for(n=0, M, print1(a(n), ", "));
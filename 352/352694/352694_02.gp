M=21;

a(n) = if(n==0, 1, sum(k=1, n, sigma(k, 2)*binomial(n-1, k-1)*a(n-k)));
for(n=0, M, print1(a(n), ", "));

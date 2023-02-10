M=17;

a(n) = sum(k=1, n, k^(n-k)*(2*n)^(k-1)*binomial(n, k));
for(n=0, M, print1(a(n), ", "));
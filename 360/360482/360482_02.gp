M=17;

a(n) = sum(k=1, n, 3^(k-1)*k^(n-1)*binomial(n, k));
for(n=0, M, print1(a(n), ", ")); 
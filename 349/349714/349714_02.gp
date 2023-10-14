M=17;

a(n) = sum(k=0, n, (3*k+1)^(n-1)*binomial(n, k))/2^n;
for(n=0, M, print1(a(n), ", "));
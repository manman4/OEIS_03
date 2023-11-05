a(n) = sum(k=0, n\4, 2^(n-3*k)*binomial(n, 4*k)*binomial(4*k, k)/(3*k+1) );
for(n=0, 26, print1(a(n), ", "))

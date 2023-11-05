a(n) = sum(k=0, n\3, 2^(n-3*k)*binomial(n, 3*k)*binomial(3*k, k)/(2*k+1) );
for(n=0, 26, print1(a(n), ", "))

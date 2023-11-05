a(n) = sum(k=0, n\2, 2^(n-k)*binomial(k+1, n-2*k)*binomial(2*k, k)/(k+1) );
for(n=0, 27, print1(a(n), ", "))

a(n) = sum(k=0, n\2, 2^k*binomial(k+1, n-2*k)*binomial(2*k, k)/(k+1));

for(n=0, 43, print1(a(n), ", "))   
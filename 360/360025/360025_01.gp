M=41;

a(n) = sum(k=0, n\3, (-1)^k*binomial(n-2*k, k)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));
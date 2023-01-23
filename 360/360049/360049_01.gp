M=33;

a(n) = sum(k=0, n\3, (-1)^k*binomial(n+2, 3*k+2)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));
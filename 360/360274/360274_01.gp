M=27;

a(n) = sum(k=0, n\3, binomial(2*(n-3*k), n-3*k)/(n-3*k+1));
for(n=0, M, print1(a(n), ", "));
M=25;

a(n) = sum(k=0, n\2, binomial(2*(n-2*k), n-2*k)/(n-2*k+1));
for(n=0, M, print1(a(n), ", "));
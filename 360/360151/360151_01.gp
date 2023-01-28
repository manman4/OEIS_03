M=25;

a(n) = sum(k=0, n\3, binomial(2*n-4*k, n-3*k));
for(n=0, M, print1(a(n),", "))

M=44;

a(n) = sum(k=0, n\10, binomial(n-5*k, 5*k));
for(n=0, M, print1(a(n), ", "));
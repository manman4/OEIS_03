\\ a(n) = sum(k=0, n\4, binomial(2*k+2, 2*n-8*k+1))/2;
a(n) = 1/2 * sum(k=0, n\4, binomial(2*k+2, 2*n-8*k+1));
for(n=0, 49, print1(a(n),", "))


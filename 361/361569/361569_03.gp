a(n) = n! * sum(k=0, n\4, binomial(4*k,n-4*k)/(24^k * k!));
for(n=0, 23, print1(a(n),", "))  

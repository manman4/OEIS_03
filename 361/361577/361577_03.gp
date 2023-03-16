a(n) = n! * sum(k=0, n\4, binomial(n-1,n-4*k)/(24^k * k!));
for(n=0, 21, print1(a(n),", "))  

a(n) = n! * sum(k=0, n\2, binomial(n-1,n-2*k)/(2^k * k!));
for(n=0, 15, print1(a(n),", "))  


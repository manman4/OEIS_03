a(n) = n! * sum(k=0, n\2, binomial(2*k,n-2*k)/(2^k * k!));
for(n=0, 21, print1(a(n),", "))  

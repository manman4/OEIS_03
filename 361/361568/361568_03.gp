a(n) = n! * sum(k=0, n\3, binomial(3*k,n-3*k)/(6^k * k!));
for(n=0, 23, print1(a(n),", "))  

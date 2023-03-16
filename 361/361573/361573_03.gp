a(n) = n! * sum(k=0, n\3, binomial(n-1,n-3*k)/(6^k * k!));
for(n=0, 21, print1(a(n),", "))  

a(n) = n! * sum(k=0, n\3, binomial(n-1,n-3*k)/k!);
for(n=0, 19, print1(a(n),", "))  

a(n) = n! * sum(k=0, n\3, binomial(3*k,n-3*k)/k!);
for(n=0, 21, print1(a(n),", "))  

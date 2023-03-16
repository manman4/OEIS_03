a(n) = n! * sum(k=0, n\4, binomial(n-1,n-4*k)/k!);
for(n=0, 19, print1(a(n),", "))  

a(n) = sum(k=0, n\2, (3*k)!/k!^3*binomial(k, n-2*k));
for(n=0, 24, print1(a(n),", "))  

a(n) = sum(k=0, n\2, (3*k)!/k!^3*binomial(3*k, n-2*k));
for(n=0, 23, print1(a(n),", "))  

a(n) = sum(k=0, n\3, (3*k)!/k!^3*binomial(k, n-3*k));
for(n=0, 30, print1(a(n),", "))  

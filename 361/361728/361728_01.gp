a(n) = sum(k=0, n, (3*k)!/k!^3*binomial(3*k, n-k));
for(n=0, 16, print1(a(n),", "))  

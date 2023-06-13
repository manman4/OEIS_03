a(n) = sumdiv(n, d, (n/d)^(n-3*n/d)*binomial(d-1, 2));
for(n=1, 35, print1(a(n),", "))  


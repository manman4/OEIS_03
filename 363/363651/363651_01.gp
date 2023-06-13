a(n) = sumdiv(n, d, (n/d)^(n-2*n/d)*binomial(d, 2));
for(n=1, 32, print1(a(n),", "))  


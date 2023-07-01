a(n) = sumdiv(n, d, (n/d%3==2)*(-1)^(n/d)*binomial(d+1, 2));

for(n=1, 76, print1(a(n),", "))  


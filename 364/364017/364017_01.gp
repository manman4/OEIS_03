a(n) = -sumdiv(n, d, (n/d%3==1)*(-1)^(n/d)*binomial(d+1, 2));

for(n=1, 51, print1(a(n),", "))  


a(n) = -sumdiv(n, d, (n/d%3==1)*(-1)^(n/d)*d);

for(n=1, 75, print1(a(n),", "))  


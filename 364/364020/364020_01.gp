a(n) = sumdiv(n, d, (n/d%5==2)*(-1)^(n/d)*d);

for(n=1, 87, print1(a(n),", "))  


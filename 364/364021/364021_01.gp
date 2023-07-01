a(n) = -sumdiv(n, d, (n/d%5==3)*(-1)^(n/d)*d);

for(n=1, 90, print1(a(n),", "))  


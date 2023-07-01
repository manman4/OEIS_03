a(n) = -sumdiv(n, d, (n/d%5==1)*(-1)^(n/d)*d);

for(n=1, 80, print1(a(n),", "))  


a(n) = sumdiv(n, d, (n/d%5==4)*(-1)^(n/d)*d);

for(n=1, 91, print1(a(n),", "))  


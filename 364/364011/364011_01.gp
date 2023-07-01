a(n) = -sumdiv(n, d, (n/d%3==1)*(-1)^(n/d));

for(n=1, 94, print1(a(n),", "))  


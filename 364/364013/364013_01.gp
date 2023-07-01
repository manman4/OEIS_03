a(n) = -sumdiv(n, d, (n/d%3==1)*(-1)^(n/d)*d^2);

for(n=1, 55, print1(a(n),", "))  


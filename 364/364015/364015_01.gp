a(n) = sumdiv(n, d, (n/d%3==2)*(-1)^(n/d)*d);

for(n=1, 79, print1(a(n),", "))  


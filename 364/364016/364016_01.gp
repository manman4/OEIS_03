a(n) = sumdiv(n, d, (n/d%3==2)*(-1)^(n/d)*d^2);

for(n=1, 69, print1(a(n),", "))  


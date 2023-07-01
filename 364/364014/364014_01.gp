a(n) = sumdiv(n, d, (n/d%3==2)*(-1)^(n/d));

for(n=1, 97, print1(a(n),", "))  


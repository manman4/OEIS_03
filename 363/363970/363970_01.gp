a(n) = sumdiv(n, d, (d%3==1)*((d+2)/3)^2);

for(n=1, 74, print1(a(n),", "))  


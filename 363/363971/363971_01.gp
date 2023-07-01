a(n) = sumdiv(n, d, (d%3==2)*((d+1)/3)^2);

for(n=1, 76, print1(a(n),", "))  


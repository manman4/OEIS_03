a(n) = sumdiv(n, d, (d%4==3)*((d+1)/4)^2);

for(n=1, 82, print1(a(n),", "))  


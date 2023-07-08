a(n) = sumdiv(4*n-2, d, (d%4==3)*(d+1))/4;

for(n=1, 87, print1(a(n),", "))  


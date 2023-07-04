a(n) = sumdiv(4*n-3, d, (d%4==3)*(d+1))/4;
for(n=1, 95, print1(a(n),", "))                                                   
 
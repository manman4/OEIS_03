a(n) = sumdiv(4*n-1, d, (d%4==1)*(d+3))/4;
for(n=1, 96, print1(a(n),", "))                                                   
 
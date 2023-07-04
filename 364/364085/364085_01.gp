a(n) = sumdiv(4*n-1, d, (d%4==3)*(d+1))/4;
for(n=1, 78, print1(a(n),", "))                                                   
 
a(n) = sumdiv(5*n-1, d, (d%5==1)*(d+4))/5;
for(n=1, 96, print1(a(n),", "))  
 
a(n) = sumdiv(5*n-4, d, (d%5==4)*(d+1))/5;
for(n=1, 95, print1(a(n),", "))  
 
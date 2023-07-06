a(n) = sumdiv(5*n-1, d, (d%5==3)*(d+2))/5;
for(n=1, 91, print1(a(n),", "))
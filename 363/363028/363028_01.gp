a(n) = sumdiv(5*n-1, d, (d%5==2)*(d+3))/5;
for(n=1, 88, print1(a(n),", "))
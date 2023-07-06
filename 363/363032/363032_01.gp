a(n) = sumdiv(5*n-4, d, (d%5==2)*(d+3))/5;
for(n=1, 93, print1(a(n),", "))
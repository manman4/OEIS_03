a(n) = sumdiv(5*n-2, d, (d%5==3)*(d+2))/5;
for(n=1, 79, print1(a(n),", "))
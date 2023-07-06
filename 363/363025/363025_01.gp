a(n) = sumdiv(5*n-2, d, (d%5==2)*d);
for(n=1, 91, print1(a(n),", "))
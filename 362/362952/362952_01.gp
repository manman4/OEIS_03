a(n) = sumdiv(5*n-1, d, (d%5==2)*d);
for(n=1, 81, print1(a(n),", "))
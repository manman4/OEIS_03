a(n) = sumdiv(5*n-4, d, (d%5==3)*d);
for(n=1, 80, print1(a(n),", "))
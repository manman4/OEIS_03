a(n) = sumdiv(n, d, (d%7==1)-(d%7==2));

for(n=1, 105, print1(a(n),", "));
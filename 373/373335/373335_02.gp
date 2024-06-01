a(n) = sumdiv(n, d, (d%5==1)-(d%5==2));

for(n=1, 101, print1(a(n),", "));
a(n) = sumdiv(n, d, (d%4==1)*(-1)^((d-1)/4));

for(n=1, 105, print1(a(n),", "))

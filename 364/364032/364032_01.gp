a(n) = sumdiv(n, d, (d%4==3)*(-1)^((d-3)/4));

for(n=1, 101, print1(a(n),", "))

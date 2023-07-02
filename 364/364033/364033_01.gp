a(n) = sumdiv(n, d, (d%4==3)*(-1)^((d-3)/4)*n/d);

for(n=1, 91, print1(a(n),", "))

a(n) = sumdiv(n, d, (d%4==1)*(-1)^((d-1)/4)*n/d);

for(n=1, 79, print1(a(n),", "))

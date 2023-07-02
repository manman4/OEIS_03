a(n) = sumdiv(n, d, (d%2==1)*2^((d-1)/2)*n/d);

for(n=1, 52, print1(a(n),", "))

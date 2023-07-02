a(n) = sumdiv(n, d, (d%2==1)*2^((d-1)/2));

for(n=1, 56, print1(a(n),", "))

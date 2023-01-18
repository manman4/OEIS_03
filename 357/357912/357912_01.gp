M=85;

a(n) = sumdiv(n, d, (Mod(d, 11)==1)*d);
for(n=1, M, print1(a(n),", "))

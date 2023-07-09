a(n) = sumdiv(n, d, (d%2==1)*d^(d+1));
for(n=1, 22, print1(a(n),", ")) 
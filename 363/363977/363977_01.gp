a(n) = sumdiv(n, d, (d%4==1)*binomial((d+3)/4+1, 2));

for(n=1, 83, print1(a(n),", "))  


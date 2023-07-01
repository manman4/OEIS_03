a(n) = sumdiv(n, d, (d%4==3)*binomial((d+1)/4+1, 2));

for(n=1, 89, print1(a(n),", "))  


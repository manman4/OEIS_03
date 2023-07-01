a(n) = sumdiv(n, d, (d%2==1)*binomial((d+1)/2+1, 2));

for(n=1, 72, print1(a(n),", "))  


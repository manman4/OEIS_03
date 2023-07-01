a(n) = sumdiv(n, d, (d%3==1)*binomial((d+2)/3+1, 2));

for(n=1, 79, print1(a(n),", "))  


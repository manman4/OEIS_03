a(n) = sumdiv(n, d, (d%3==2)*binomial((d+1)/3+1, 2));

for(n=1, 79, print1(a(n),", "))  


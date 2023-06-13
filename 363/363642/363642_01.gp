a(n) = sumdiv(n, d, (n/d)^(d-1)*binomial(d+1, 2));
for(n=1, 43, print1(a(n),", "))  
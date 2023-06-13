a(n) = sumdiv(n, d, (n/d)^(d-1)*binomial(d+2, 3));
for(n=1, 38, print1(a(n),", "))  
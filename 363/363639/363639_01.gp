a(n) = sumdiv(n, d, (n/d)^d*binomial(d+2, 2));
for(n=1, 39, print1(a(n),", "))  
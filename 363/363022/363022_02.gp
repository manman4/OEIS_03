a(n) = sumdiv(n, d, (-1)^d*binomial(d, 2));
for(n=1, 53, print1(a(n),", "))  
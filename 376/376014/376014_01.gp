\\ a(n) = Sum_{d|n} d^d * binomial(n/d,d).
a(n) = sumdiv(n, d, d^d * binomial(n/d, d));
for(n=1, 51, print1(a(n),", ")) 

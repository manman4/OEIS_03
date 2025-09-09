\\ a(n) = Sum_{k=0..n} binomial(5*n+4,5*k+3).
a(n) = sum(k=0, n, binomial(5*n+4, 5*k+3));
for(n=0, 16, print1(a(n),", "));



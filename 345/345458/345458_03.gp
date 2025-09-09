\\ a(n) = Sum_{k=0..n} binomial(5*n+4,5*k+4).
a(n) = sum(k=0, n, binomial(5*n+4, 5*k+4));
for(n=0, 20, print1(a(n),", "));


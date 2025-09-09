\\ a(n) = Sum_{k=0..n} binomial(5*n+2,5*k+2).
a(n) = sum(k=0, n, binomial(5*n+2, 5*k+2));
for(n=0, 20, print1(a(n),", "));


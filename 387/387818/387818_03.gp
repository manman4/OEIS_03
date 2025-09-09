\\ a(n) = (1/2) * Sum_{k=0..n} binomial(7*n+5,7*k+3).
b(n) = sum(k=0, n, binomial(7*n+5, 7*k+3))/2;
for(n=0, 14, print1(b(n),", "));

\\ a(n) = (1/4) * Sum_{k=0..n} binomial(7*n+6,7*k+3).
a(n) = sum(k=0, n, binomial(7*n+6, 7*k+3))/4;
for(n=0, 20, print1(a(n)-b(n),", "));

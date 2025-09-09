\\ a(n) = Sum_{k=0..n} binomial(7*n+3,7*k+2).
b(n) = sum(k=0, n, binomial(7*n+3, 7*k+2));
for(n=0, 14, print1(b(n),", "));

\\ a(n) = (1/2) * Sum_{k=0..n} binomial(7*n+4,7*k+2).
a(n) = sum(k=0, n, binomial(7*n+4, 7*k+2))/2;
for(n=0, 20, print1(a(n)-b(n),", "));

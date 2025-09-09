\\ a(n) = Sum_{k=0..n} binomial(5*n+3,5*k+2).
b(n) = sum(k=0, n, binomial(5*n+3, 5*k+2));
for(n=0, 16, print1(b(n),", "));

\\ a(n) = (1/2) * Sum_{k=0..n} binomial(5*n+4,5*k+2).
a(n) = sum(k=0, n, binomial(5*n+4, 5*k+2))/2;
for(n=0, 20, print1(a(n)-b(n),", "));

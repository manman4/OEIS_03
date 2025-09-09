\\ a(n) = Sum_{k=0..n} binomial(5*n+1,5*k+1).
a(n) = sum(k=0, n, binomial(5*n+1, 5*k+1));
for(n=0, 20, print1(a(n),", "));

\\ a(n) = (1/2) * Sum_{k=0..n} binomial(5*n+2,5*k+1).
b(n) = (1/2) * sum(k=0, n, binomial(5*n+2, 5*k+1));
for(n=0, 50, print1(a(n)-b(n),", "));
\\ a(n) = (1/3) * Sum_{k=0..n} binomial(6*n+3,6*k+2).
b(n) = sum(k=0, n, binomial(6*n+3, 6*k+2))/3;
for(n=0, 16, print1(b(n),", "));

\\ a(n) = (1/6) * Sum_{k=0..n} binomial(6*n+4,6*k+2).
a(n) = sum(k=0, n, binomial(6*n+4, 6*k+2))/6;
for(n=0, 20, print1(a(n)-b(n),", "));

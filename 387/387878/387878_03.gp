\\ a(n) = Sum_{k=0..n} binomial(6*n+4,6*k+4).
b(n) = sum(k=0, n, binomial(6*n+4, 6*k+4));
for(n=0, 15, print1(b(n),", "));

\\ a(n) = (-1 + 16*64^n - 9*(-27)^n)/6.
a(n) = (-1 + 16*64^n - 9*(-27)^n)/6;
for(n=0, 25, print1(a(n)-b(n),", "));



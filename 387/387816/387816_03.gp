\\ a(n) = Sum_{k=0..n} binomial(7*n+1,7*k+1).
b(n) = sum(k=0, n, binomial(7*n+1, 7*k+1));
for(n=0, 14, print1(b(n),", "));

\\ a(n) = (1/2) * Sum_{k=0..n} binomial(7*n+2,7*k+1).
a(n) = sum(k=0, n, binomial(7*n+2, 7*k+1))/2;
for(n=0, 20, print1(a(n)-b(n),", "));

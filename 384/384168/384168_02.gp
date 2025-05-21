M=17;

\\ a(n) = 3^n * n! * binomial(4*n/3,n) * Sum_{k=1..n} 1/(n+3*k).
a(n) = 3^n * n! * binomial(4*n/3,n) * sum(k=1, n, 1/(n+3*k));
for(n=1, M, print1(a(n),", "));

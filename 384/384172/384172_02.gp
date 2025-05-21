M=17;

\\ a(n) = 4^n * n! * binomial(7*n/4,n) * Sum_{k=1..n} 1/(3*n+4*k).
a(n) = 4^n * n! * binomial(7*n/4,n) * sum(k=1, n, 1/(3*n+4*k));
for(n=1, M, print1(a(n),", "));

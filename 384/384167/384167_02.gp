M=17;

\\ a(n) = 2^n * n! * binomial(3*n/2,n) * Sum_{k=1..n} 1/(n+2*k).
a(n) = 2^n * n! * binomial(3*n/2,n) * sum(k=1, n, 1/(n+2*k));
for(n=1, M, print1(a(n),", "));

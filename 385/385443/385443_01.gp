M=19;

\\ a(n) = 6^n * n! * binomial((4*n+1)/6,n)/(4*n+1).
a(n) = 6^n * n! * binomial((4*n+1)/6, n)/(4*n+1);
for(n=0, M, print1(a(n),", "));

a385343(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(asin(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} (n+1)^(k-1) * (3*i)^(n-k) * A385343(n,k), where i is the imaginary unit.
b(n) = sum(k=0, n, (n+1)^(k-1) * (3*I)^(n-k) * a385343(n, k));
for(n=0, M, print1(a(n)-b(n),", "));


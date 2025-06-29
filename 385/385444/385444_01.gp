M=20;

\\ a(n) = 8^n * n! * binomial((5*n+1)/8,n)/(5*n+1).
a(n) = 8^n * n! * binomial((5*n+1)/8, n)/(5*n+1);
for(n=0, M, print1(a(n),", "));

a385343(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(asin(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} (n+1)^(k-1) * (4*i)^(n-k) * A385343(n,k), where i is the imaginary unit.
b(n) = sum(k=0, n, (n+1)^(k-1) * (4*I)^(n-k) * a385343(n, k));
for(n=0, M, print1(a(n)-b(n),", "));

\\ a(8*n+3) = 0 for n >= 0.
for(n=0, 100, if(a(8*n+3) != 0, print1(n,", ")));
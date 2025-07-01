M=19;

\\ a(n) = n!/2^n * A138020(n) = n! * Sum_{k=0..n} binomial(n,k) * binomial(n/2+k+1/2,n)/(n+2*k+1).
b(n) = n! * sum(k=0, n, binomial(n, k) * binomial(n/2 + k + 1/2, n) / (n + 2*k + 1));
for(n=0, M, print1(b(n),", "));

a111594(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(atanh(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} (n+1)^(k-1) * A111594(n,k).
a(n) = sum(k=0, n, (n+1)^(k-1) * a111594(n, k));
for(n=0, M, print1(a(n)-b(n),", "));
M=29;

T(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(asin(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} i^(n-k) * A385343(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, I^(n-k) * T(n, k));
for(n=0, M, print1(a(n),", "));

\\ a177698(n) = n! * binomial(n-1,(n-1)/2) * 2^(1-n) * (-1)^((n-1)/2) if n is odd > 0.
a177698(n) = if(n%2==0, 0, n! * binomial(n-1, (n-1)/2) * 2^(1-n) * (-1)^((n-1)/2));
\\ a(n) = A177698(n-1) for n > 1.
for(n=1, M, print1(a177698(n-1),", "));

\\ a(2*n+1) = 0 for n > 0.
for(n=0, M, print1(a(2*n+1),", "));
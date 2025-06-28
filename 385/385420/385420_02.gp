M=19;

T(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(asin(x)^k/k!, n);
a007559(n) = prod(k=0, n-1, 3*k+1);
\\ a(n) = Sum_{k=0..n} A007559(k) * (3*i)^(n-k) * A385343(n,k), where i is the imaginary unit.
a(n) = sum(k=0, n, a007559(k) * (3*I)^(n-k) * T(n, k));
for(n=0, M, print1(a(n),", "));
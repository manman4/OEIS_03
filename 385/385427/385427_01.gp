M=21;

a385343(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(asin(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} (n-k+1)^(k-1) * A385343(n,k).
a(n) = sum(k=0, n, (n-k+1)^(k-1) * a385343(n, k));
for(n=0, M, print1(a(n),", "));
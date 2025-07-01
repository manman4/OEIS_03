M=19;

a111594(n, k) = my(x='x+O('x^(n+1))); n!*polcoef(atanh(x)^k/k!, n);
\\ a(n) = Sum_{k=0..n} (k+1)^(k-1) * A111594(n,k).
a(n) = sum(k=0, n, (k+1)^(k-1) * a111594(n, k));
for(n=0, M, print1(a(n),", "));
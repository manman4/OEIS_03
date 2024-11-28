a005810(n) = binomial(4*n, n);
\\ a(n) = Sum_{k=0..n} C(4*k,k)*C(4*(n-k),n-k).
a078995(n) = sum(k=0, n, binomial(4*k, k)*binomial(4*(n-k), n-k));

\\ 27*a(n) - 256*a(n-1) = 18*A078995(n) + 8*A005810(n) for n > 0.
a(n) = if(n==0, 1, (18*a078995(n) + 8*a005810(n) + 256*a(n-1))/27);
for(n=0, 20, print1(a(n), ", "));
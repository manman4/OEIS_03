a005810(n) = binomial(4*n, n);

\\ A337291 a(n) = 3*binomial(4*n,n)/(4*n-1).
a337291(n) = 3*binomial(4*n, n)/(4*n-1);

\\ 27*a(n) - 256*a(n-1) = 18*A005810(n) - A337291(n) for n > 0.
a(n) = if(n==0, 1, (18*a005810(n) - a337291(n) + 256*a(n-1))/27);
for(n=0, 20, print1(a(n), ", "));
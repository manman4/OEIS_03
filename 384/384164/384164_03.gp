M=17;

\\ a(n) = (3/4) * A061924(n) for n > 0.
a(n) = if(n==0, 1, (3/4) * n! * binomial(4*n,n));
for(n=0, M, print1(a(n),", "));

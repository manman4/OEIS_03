M=16;

\\ a(n) = n! * binomial(4*n-3,n).
a(n) = n! * binomial(4*n-3,n);
for(n=0, M, print1(a(n),", "));

M=16;

\\ n! * binomial(4*n-1,n).
a(n) = n! * binomial(4*n-1,n);
for(n=0, M, print1(a(n),", "));

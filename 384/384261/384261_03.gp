M=16;

\\ a(n) = n! * binomial(3*n-2,n).
a(n) = n! * binomial(3*n-2,n);
for(n=0, M, print1(a(n),", "));

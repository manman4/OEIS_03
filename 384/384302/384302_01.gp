M=12;

\\ a(n) = (2*n)! * binomial(5*n-1,2*n).
a(n) = (2*n)! * binomial(5*n-1, 2*n);
for(n=0, M, print1(a(n),", "));

M=17;

\\ a(n) = (3/5) * 2^n * n! * binomial(5*n/2,n) for n > 0.
a(n) = if(n==0, 1, (3/5) * 2^n * n! * binomial(5*n/2,n));
for(n=0, M, print1(a(n),", "));

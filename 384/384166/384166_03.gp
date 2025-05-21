M=17;

\\ a(n) = (3/7) * 4^n * n! * binomial(7*n/4,n) for n > 0.
a(n) = if(n==0, 1, (3/7) * 4^n * n! * binomial(7*n/4,n));
for(n=0, M, print1(a(n),", "));

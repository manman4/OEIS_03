M=17;

\\ a(n) = (2/5) * 3^n * n! * binomial(5*n/3,n) for n > 0.
a(n) = if(n==0, 1, (2/5) * 3^n * n! * binomial(5*n/3,n));
for(n=0, M, print1(a(n),", "));

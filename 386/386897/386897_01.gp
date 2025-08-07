M=17;

\\ a(n) = 4^n * binomial(5*n/2,n).
a(n) = 4^n * binomial(5*n/2,n);
for(n=0, M, print1(a(n)", "));

\\ a(n) == 0 (mod 10) for n > 0.
for(n=0, 1000, if(a(n) % 10 != 0, print1(n", ")));
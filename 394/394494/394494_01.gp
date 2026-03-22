M=18;

\\ a(n) = 4^n * n! * binomial((5*n-3)/4,n)/(n+1).
a(n) = 4^n * n! * binomial((5*n-3)/4, n) / (n+1);
for(n=0, M, print1(a(n),", "));


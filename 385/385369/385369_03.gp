M=29;

\\ a(n) = 2^n * n! * binomial((n+1)/2,n)/(n+1).
a(n) = 2^n * n! * binomial((n+1)/2, n)/(n+1);
for(n=0, M, print1(a(n),", "));




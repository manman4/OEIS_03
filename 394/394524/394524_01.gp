M=16;

\\ a(n) = 4^n * n! * binomial((8*n+1)/4,n)/(8*n+1).
a(n) = 4^n * n! * binomial((8*n+1)/4, n) / (8*n+1);
for(n=0, M, print1(a(n),", "));


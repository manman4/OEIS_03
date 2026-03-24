M=22;

\\ a(n) = 5^n * n! * binomial((2*n+1)/5,n)/(2*n+1).
a(n) = 5^n * n! * binomial((2*n+1)/5, n) / (2*n+1);
for(n=0, M, print1(a(n),", "));


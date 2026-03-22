M=18;

\\ a(n) = 3^n * n! * binomial((4*n-2)/3,n)/(n+1).
a(n) = 3^n * n! * binomial((4*n-2)/3, n) / (n+1);
for(n=0, M, print1(a(n),", "));


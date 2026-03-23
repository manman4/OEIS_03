M=17;

\\ a(n) = 3^n * n! * binomial((6*n+1)/3,n)/(6*n+1).
a(n) = 3^n * n! * binomial((6*n+1)/3, n) / (6*n+1);
for(n=0, M, print1(a(n),", "));


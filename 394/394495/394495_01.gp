M=17;

\\ a(n) = 5^n * n! * binomial((6*n-4)/5,n)/(n+1).
a(n) = 5^n * n! * binomial((6*n-4)/5, n) / (n+1);
for(n=0, M, print1(a(n),", "));


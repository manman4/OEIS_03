M=23;

\\ a(n) = 5^n * n! * binomial((3*n+1)/5,n)/(3*n+1).
a(n) = 5^n * n! * binomial((3*n+1)/5, n) / (3*n+1);
for(n=0, M, print1(a(n),", "));

\\ a(5*n+3) = 0.
for(n=0, 1000, if(a(5*n+3)!=0, print1(n, ", ")));


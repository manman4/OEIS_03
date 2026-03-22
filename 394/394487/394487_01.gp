M=23;

\\ a(n) = 4^n * n! * binomial((n+1)/4,n)/(n+1).
a(n) = 4^n * n! * binomial((n+1)/4, n) / (n+1);
for(n=0, M, print1(a(n),", "));

\\ a(4*n+3) = 0.
for(n=0, 5000, if(a(4*n+3)!=0, print1(n,", ")));
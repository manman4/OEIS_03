M=26;

\\ a(n) = 3^n * n! * binomial(2*(n+1)/3,n)/(n+1).
a(n) = 3^n * n! * binomial(2*(n+1)/3, n) / (n+1);
for(n=0, M, print1(a(n),", "));

\\ a(3*n+2) = 0 for n > 0.
for(n=0, 5000, if(a(3*n+2)!=0, print1(n,", ")));
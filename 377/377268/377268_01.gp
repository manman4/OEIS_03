M=30;

\\ a(n) = 9^n * binomial(2*n/3 - 4/3,n)/(n+1).
a(n) = 9^n * binomial(2*n/3 - 4/3,n)/(n+1);
for(n=0, M, print1(a(n), ", ")) 

\\ a(3*n+2) = 0 for n >= 0.
for(n=0, 100, if(a(3*n+2) != 0, print1(n, ", ")));
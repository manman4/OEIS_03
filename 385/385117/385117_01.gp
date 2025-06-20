M=28;

\\ a(n) = 9^n * binomial(2*n/3+1,n)/(2*n/3+1).
a(n) = 9^n * binomial(2*n/3+1,n)/(2*n/3+1);
for(n=0, M, print1(a(n),", "));

\\ a(3*n) = 0 for n > 1.
for(n=0, 100, if(a(3*n) != 0, print1(n, ", ")));
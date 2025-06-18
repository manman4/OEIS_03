M=28;

\\ a(n) = 9^n * binomial(2*n/3+1,n)/(2*n/3+1).
a(n) = 9^n * binomial(2*n/3+1,n)/(2*n/3+1);
for(n=0, M, print1(a(n),", "));
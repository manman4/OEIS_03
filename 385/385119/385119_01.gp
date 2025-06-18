M=17;

\\ a(n) = 9^n * binomial(5*n/3+1,n)/(5*n/3+1).
a(n) = 9^n * binomial(5*n/3+1,n)/(5*n/3+1);
for(n=0, M, print1(a(n),", "));
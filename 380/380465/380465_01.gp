M=17;

\\ a(n) = 25^n * binomial(7*n/5+1/5,n)/(7*n+1).
a(n) = 25^n * binomial(7*n/5+1/5,n)/(7*n+1);
for(n=0, M, print1(a(n), ", ")) 


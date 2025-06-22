M=29;

\\ a(n) = 9^n * binomial(n/3 - 5/3,n)/(n+1).
a(n) = 9^n * binomial(n/3 - 5/3,n)/(n+1);
for(n=0, M, print1(a(n), ", ")) 

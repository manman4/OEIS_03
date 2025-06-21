M=16;

\\ a(n) = 49^n * binomial(8*n/7+1/7,n)/(8*n+1).
a(n) = 49^n * binomial(8*n/7+1/7,n)/(8*n+1);
for(n=0, M, print1(a(n), ", ")) 

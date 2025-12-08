M=21;
\\ a(n) = binomial(5*n+1,n)/(5*n+1) + binomial(5*n-1,n)/(5*n-1) for n > 0.
a(n) = binomial(5*n+1,n)/(5*n+1) + binomial(5*n-1,n)/(5*n-1);
for(n=0, M, print1(a(n),", "));

a(n) = if(n==0, 1, binomial(5*n+1,n)/(5*n+1) + binomial(5*n-1,n)/(5*n-1));
for(n=0, M, print1(a(n),", "))  
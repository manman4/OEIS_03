M=23;
\\ a(n) = binomial(3*n+1,n)/(3*n+1) + binomial(3*n-1,n)/(3*n-1) for n > 0.
a(n) = binomial(3*n+1,n)/(3*n+1) + binomial(3*n-1,n)/(3*n-1);
for(n=0, M, print1(a(n),", "));

a(n) = if(n==0, 1, binomial(3*n+1,n)/(3*n+1) + binomial(3*n-1,n)/(3*n-1));
for(n=0, M, print1(a(n),", "))  
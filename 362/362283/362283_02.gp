M=21;

a034940(n) = (2*n+1)^n*(2*n)!/(2^n*n!); 
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, a034940(k) * binomial(n-1,2*k) * a(n-2*k-1)));
for(n=0, M, print1(a(n), ", ")); 
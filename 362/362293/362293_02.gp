M=21;

a362292(n) = (n+1/3)^n*(3*n)!/n!;
a(n) = if(n==0, 1, sum(k=0, (n-1)\3, a362292(k) * binomial(n-1,3*k) * a(n-3*k-1)));
for(n=0, M, print1(a(n), ", ")); 

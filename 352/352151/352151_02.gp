M=20;

c(n) = ((-4)^n-(-16)^n)*bernfrac(2*n)/(2*n);
b(n) = if(n%2==1, c((n+1)/2), (-1)^(n/2+1));
a(n) = if(n==0, 1, sum(k=1, n, b(k)*binomial(n, k)*a(n-k)));
for(n=0, M, print1(a(n), ", "));
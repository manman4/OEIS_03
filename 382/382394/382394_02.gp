b(n)=if(n<1, 0, sum(k=0, n, (n-k)*binomial(n+k-1, k)^2/n));
\\ a(n) = binomial(2*n,n)/2 * A183069(n) for n > 0.
a(n) = binomial(2*n,n)/2 * b(n);
for(n=0, 17, print1(a(n), ", "))

c(n)=sum(k=0, n\2, (binomial(n, k)-binomial(n, k-1))^3);
\\ a(n) = A003161(2*n-1) for n > 0.
a(n) = c(2*n-1);
for(n=0, 17, print1(a(n), ", "))


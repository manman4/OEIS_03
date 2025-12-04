\\ a(n) = 3*4^(n-2)/2 + binomial(2*n-2,n) + (n-1)*binomial(2*n-4,n-2)/2 for n > 1.
a(n) = if(n<2, 0^n, 3*4^(n-2)/2+binomial(2*n-2, n)+(n-1)*binomial(2*n-4, n-2)/2);
for(n=0, 26, print1(a(n), ", "));

\\ a(n) = (6/n) * Sum_{k=0..floor(n/2)} binomial(k+2,3) * binomial(2*n,n-2*k) for n > 0.
b(n) = if(n==0, 1, 6/n * sum(k=0, n\2, binomial(k+2, 3) * binomial(2*n, n-2*k)));
for(n=0, 50, print1(a(n)-b(n), ", "));




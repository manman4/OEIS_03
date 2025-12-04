\\ a(n) = (6/n) * Sum_{k=0..floor(n/2)} binomial(k+2,3) * binomial(3*n,n-2*k) for n > 0.
a(n) = if(n==0, 1, 6/n * sum(k=0, n\2, binomial(k+2, 3) * binomial(3*n, n-2*k)));
for(n=0, 23, print1(a(n), ", "));




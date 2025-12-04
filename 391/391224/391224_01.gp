\\ a(n) = (8/n) * Sum_{k=0..floor(n/2)} binomial(k+3,4) * binomial(4*n,n-2*k) for n > 0.
a(n) = if(n==0, 1, 8/n * sum(k=0, n\2, binomial(k+3, 4) * binomial(4*n, n-2*k)));
for(n=0, 21, print1(a(n), ", "));




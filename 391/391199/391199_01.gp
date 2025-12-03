\\ a(n) = Sum_{k=0..n} (k+1)^2 * binomial(4*n-2*k+2,n-k)/(2*n-k+1).
a(n) = sum(k=0, n, (k+1)^2 * binomial(4*n-2*k+2, n-k)/(2*n-k+1));
for(n=0, 21, print1(a(n), ", "));




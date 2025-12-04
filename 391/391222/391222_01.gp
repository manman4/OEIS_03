\\ a(n) = 4^(n-2) + binomial(2*n-2,n) for n > 1.
a(n) = if(n<2, 0^n, 4^(n-2)+binomial(2*n-2, n));
for(n=0, 26, print1(a(n), ", "));

\\ a(n) = (4/n) * Sum_{k=0..floor(n/2)} binomial(k+1,2) * binomial(2*n,n-2*k) for n > 0.
b(n) = if(n==0, 1, 4/n * sum(k=0, n\2, binomial(k+1, 2) * binomial(2*n, n-2*k)));
for(n=0, 50, print1(a(n)-b(n), ", "));




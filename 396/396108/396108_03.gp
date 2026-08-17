M=18;
\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/2)} binomial(n,2*k+1) * a(n-2*k-1) + 2 * Sum_{k=1..floor(n/2)} 4^(k-1) * binomial(n,2*k) * a(n-2*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\2, binomial(n, 2*k+1) * a(n-2*k-1))) + 2 * sum(k=1, n\2, 4^(k-1) * binomial(n, 2*k) * a(n-2*k));
for(n=0, M, print1(a(n), ", "));


\\ a(0) = 1; a(n) = 5 * Sum_{k=1..n} 2^(k-1) * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, 5 * sum(k=1, n, 2^(k-1) * binomial(n, k) * a(n-k)));
for(n=0, 15, print1(a(n),", "));

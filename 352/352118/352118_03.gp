M=17;

\\ a(0) = 1; a(n) = Sum_{k=1..n} 3^k * (1 - 2/3 * k/n) * binomial(n,k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, 3^k * (1 - 2/3 * k/n) * binomial(n,k) * a(n-k)));
for(n=0, M, print1(a(n), ", "));
\\ a(0) = 1; a(n) = (1/2) * Sum_{k=1..n} (A002104(k) - A002741(k)) * binomial(n,k) * a(n-k).
a002104(n) = sum(k=0, n-1,          binomial(n, k) * (n-k-1)!);
a002741(n) = sum(k=0, n-1, (-1)^k * binomial(n, k) * (n - k - 1)!);
a(n) = if(n==0, 1, (1/2) * sum( k=1, n, (a002104(k) - a002741(k)) * binomial(n, k) * a(n-k)));
for(n=0, 17, print1(a(n), ", "));
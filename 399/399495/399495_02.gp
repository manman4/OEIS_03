\\ a(0) = 1; a(n) = Sum_{k=1..n} ((k-1)! - A002741(k)) * binomial(n-1,k-1) * a(n-k).
a002741(n) = if(n<0, 0, sum(k=0, n-1, (-1)^k * binomial(n, k) * (n - k - 1)!));
a(n) = if(n==0, 1, sum(k=1, n, ((k-1)! - a002741(k)) * binomial(n-1, k-1) * a(n-k)));
for(n=0, 17, print1(a(n), ", "));
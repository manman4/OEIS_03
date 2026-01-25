\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/4)} (3*k)! * binomial(n+1,k) * |Stirling1(n-k,3*k)|/(n-k)!.
a(n) = (n!/(n+1)) * sum(k=0, n\4, (3*k)! * binomial(n+1,k) * abs(stirling(n-k, 3*k, 1))/(n-k)!);
for(n=0, 20, print1(a(n), ", "));


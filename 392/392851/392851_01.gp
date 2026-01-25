\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/4)} (3*k)! * binomial(n+1,k) * Stirling2(n-k,3*k)/(n-k)!.
a(n) = (n!/(n+1)) * sum(k=0, n\4, (3*k)! * binomial(n+1,k) * stirling(n-k, 3*k, 2)/(n-k)!);
for(n=0, 21, print1(a(n), ", "));


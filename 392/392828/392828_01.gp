\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/5)} (3*k)! * binomial(n+1,k) * Stirling2(n-2*k,3*k)/(n-2*k)!.
a(n) = n!*sum(k=0, n\5, (3*k)! * binomial(n+1,k) * stirling(n-2*k, 3*k, 2)/(n-2*k)!)/(n+1);
for(n=0, 21, print1(a(n), ", "));


\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/3)} (2*k)! * binomial(n+1,k) * Stirling2(n-k,2*k)/(n-k)!.
a(n) = n!*sum(k=0, n\3, (2*k)! * binomial(n+1,k) * stirling(n-k, 2*k, 2)/(n-k)!)/(n+1);
for(n=0, 20, print1(a(n), ", "));


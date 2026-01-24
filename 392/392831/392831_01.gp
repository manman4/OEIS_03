\\ a(n) = (n!/(n+1)) * Sum_{k=0..floor(n/4)} (2*k)! * binomial(n+1,k) * |Stirling1(n-2*k,2*k)|/(n-2*k)!.
a(n) = n!*sum(k=0, n\4, (2*k)! * binomial(n+1,k) * abs(stirling(n-2*k, 2*k, 1))/(n-2*k)!)/(n+1);
for(n=0, 21, print1(a(n), ", "));


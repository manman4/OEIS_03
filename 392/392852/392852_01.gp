\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (2*k)! * binomial(n+1,k) * |Stirling1(n,2*k)|.
a(n) = (1/(n+1)) * sum(k=0, n\2, (2*k)! * binomial(n+1,k) * abs(stirling(n, 2*k, 1)));
for(n=0, 19, print1(a(n), ", "));


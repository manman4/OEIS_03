\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (3*k)! * binomial(n+1,k) * |Stirling1(n,3*k)|.
a(n) = (1/(n+1)) * sum(k=0, n\3, (3*k)! * binomial(n+1,k) * abs(stirling(n, 3*k, 1)));
for(n=0, 20, print1(a(n), ", "));


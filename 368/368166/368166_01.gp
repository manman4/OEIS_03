\\ a(n) = n! * Sum_{k=1..floor(n/4)} (k-1)! * |Stirling1(n-3*k,k)|/(6^k * (n-3*k)!).
a(n) = n! * sum(k=1, n\4, (k-1)! * abs(stirling(n-3*k,k,1))/(6^k * (n-3*k)!));
for(n=0, 23, print1(a(n), ", "));


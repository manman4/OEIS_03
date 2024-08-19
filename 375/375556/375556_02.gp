\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k)! * |Stirling1(k,n-3*k)|/(6^k*k!).
a(n) = n! * sum(k=0, n\3, (n-3*k)!*abs(stirling(k, n-3*k, 1))/(6^k*k!));
for(n=0, 24, print1(a(n),", "))   
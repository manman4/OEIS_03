\\ a(n) = n! * Sum_{k=0..floor(n/4)} (n-4*k)! * |Stirling1(k,n-4*k)|/(24^k*k!).
a(n) = n! * sum(k=0, n\4, (n-4*k)!*abs(stirling(k, n-4*k, 1))/(24^k*k!));
for(n=0, 26, print1(a(n),", "))   
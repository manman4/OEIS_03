\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k)! * |Stirling1(k,n-3*k)|/k!.
a(n) = n! * sum(k=0, n\3, (n-3*k)!*abs(stirling(k, n-3*k, 1))/k!);
for(n=0, 22, print1(a(n),", "))   
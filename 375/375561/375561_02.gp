\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n-2*k)! * |Stirling1(k,n-2*k)|/k!.
a(n) = n! * sum(k=0, n\2, (n-2*k)!*abs(stirling(k, n-2*k, 1))/k!);
for(n=0, 22, print1(a(n),", "))   
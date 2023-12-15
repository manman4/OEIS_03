\\ a(n) = n! * Sum_{k=1..floor(n/4)} (k-1)! * Stirling2(n-3*k,k)/(n-3*k)!.
a(n) = n! * sum(k=1, n\4, (k-1)! * stirling(n-3*k,k,2)/(n-3*k)!);
for(n=0, 22, print1(a(n), ", "));
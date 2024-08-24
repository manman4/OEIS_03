\\ a(n) = n! * Sum_{k=0..floor(n/3)} (k+1)! * |Stirling1(n-2*k,k)|/(n-2*k)!.
a(n) = n! * sum(k=0, n\3, (k+1)! * abs(stirling(n-2*k, k))/(n-2*k)!);
for(n=0, 20, print1(a(n), ", "))

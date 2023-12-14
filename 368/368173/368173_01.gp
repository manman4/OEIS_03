\\ a(n) = n! * Sum_{k=1..floor(n/3)} (k-1)! * Stirling2(n-2*k,k)/(2^k * (n-2*k)!).
a(n) = n! * sum(k=1, n\3, (k-1)! * stirling(n-2*k,k,2)/(2^k * (n-2*k)!));
for(n=0, 23, print1(a(n), ", "));


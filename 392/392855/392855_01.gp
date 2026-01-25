\\ a(n) = n! * Sum_{k=0..floor(n/3)} (2*k)! * binomial(2*n-k+1,k)/(2*n-k+1) * Stirling2(n-k,2*k)/(n-k)!.
a(n) = n!*sum(k=0, n\3, (2*k)! * binomial(2*n-k+1,k)/(2*n-k+1) * stirling(n-k, 2*k, 2)/(n-k)!);
for(n=0, 20, print1(a(n), ", "));


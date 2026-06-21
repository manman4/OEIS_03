M=23;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (2*n-4*k)! * Stirling2(k,n-4*k)/k!.
a(n) = (1/(n+1)) * sum(k=0, n\4, (2*n-4*k)!/k! * stirling(k, n-4*k, 2));
for(n=0, M, print1(a(n), ", "));

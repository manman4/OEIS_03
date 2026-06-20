M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/3)..floor(n/2)} (3*k)!/k! * Stirling2(k,3*k-n).
a(n) = (1/(n+1)) * sum(k=ceil(n/3), n\2, (3*k)!/k! * stirling(k, 3*k-n, 2));
for(n=0, M, print1(a(n), ", "));

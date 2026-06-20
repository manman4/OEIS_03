M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=ceiling(n/4)..floor(n/3)} (4*k)!/k! * Stirling2(k,4*k-n).
a(n) = (1/(n+1)) * sum(k=ceil(n/4), n\3, (4*k)!/k! * stirling(k, 4*k-n, 2));
for(n=0, M, print1(a(n), ", "));

\\ a(n) = Sum_{k=0..floor((n-1)/2)} (3*k)!/k! * Stirling2(n+k,3*k+1).
a(n) = sum(k=0, (n-1)\2, (3*k)!/k! * stirling(n+k, 3*k+1, 2));
for(n=1, 19, print1(a(n), ", "));


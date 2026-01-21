\\ a(n) = Sum_{k=0..floor((n-1)/2)} (3*k)!/k! * Stirling2(n-1+k,3*k).
a(n) = sum(k=0, (n-1)\2, (3*k)!/k! * stirling(n-1+k, 3*k, 2));
for(n=1, 19, print1(a(n), ", "));


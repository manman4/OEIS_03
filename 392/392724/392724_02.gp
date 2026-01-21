\\ a(n) = Sum_{k=0..floor((n-1)/3)} (4*k)!/k! * Stirling2(n-1+k,4*k).
a(n) = sum(k=0, (n-1)\3, (4*k)!/k! * stirling(n-1+k, 4*k, 2));
for(n=1, 20, print1(a(n), ", "));


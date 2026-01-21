\\ a(n) = Sum_{k=0..floor((n-1)/2)} (3*k)!/k! * |Stirling1(n-1+k,3*k)|.
a(n) = sum(k=0, (n-1)\2, (3*k)!/k! * abs(stirling(n-1+k, 3*k, 1)));
for(n=1, 19, print1(a(n), ", "));


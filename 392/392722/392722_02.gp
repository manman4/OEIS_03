\\ a(n) = Sum_{k=0..floor((n-1)/3)} (4*k)!/k! * |Stirling1(n-1+k,4*k)|.
a(n) = sum(k=0, (n-1)\3, (4*k)!/k! * abs(stirling(n-1+k, 4*k, 1)));
for(n=1, 20, print1(a(n), ", "));


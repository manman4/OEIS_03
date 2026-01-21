\\ a(n) = Sum_{k=0..floor((n-1)/3)} (4*k)!/k! * |Stirling1(n+k,4*k+1)|.
a(n) = sum(k=0, (n-1)\3, (4*k)!/k! * abs(stirling(n+k, 4*k+1, 1)));
for(n=1, 20, print1(a(n), ", "));


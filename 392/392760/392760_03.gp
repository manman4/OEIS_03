\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (2*k)!/k! * (n+k)! * |Stirling1(n-k,2*k)|/(n-k)!.
a(n) = sum(k=0, n\3, (2*k)!/k! * (n+k)! * abs(stirling(n-k, 2*k, 1))/(n-k)!)/(n+1);
for(n=0, 20, print1(a(n), ", "));


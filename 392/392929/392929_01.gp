\\ a(n) = n! * Sum_{k=0..floor(n/2)} 1/(k+1)! * |Stirling1(n-k,n-2*k)|.
a(n) = n! * sum(k=0, n\2, 1 / (k+1)! * abs(stirling(n-k, n-2*k, 1)));
for(n=0, 20, print1(a(n), ", "));


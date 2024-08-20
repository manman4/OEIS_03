\\ a(n) = n! * Sum_{k=0..floor(n/3)} Stirling2(k,n-3*k)/(6^k*k!).
a(n) = n! * sum(k=0, n\3, stirling(k, n-3*k, 2)/(6^k*k!));
for(n=0, 25, print1(a(n),", "))   
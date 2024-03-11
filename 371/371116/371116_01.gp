M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} Stirling2(n-2*k,k)/(n-3*k+1)!.
a(n) = n! * sum(k=0, n\3, stirling(n-2*k,k,2)/(n-3*k+1)!);
for(n=0, M, print1(a(n)", "))
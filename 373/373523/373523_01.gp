M=23;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} binomial(2*n/3-2*k,k)/(n-3*k)!.
a(n) = n! * sum(k=0, n\3, binomial(2*n/3-2*k,k)/(n-3*k)!);
for(n=0, M, print1(a(n), ", "))
for(n=0, 200, print1((a(n)-1)%16,", "))  
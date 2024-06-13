\\ a(n) = n! * Sum_{k=0..floor(n/3)} binomial(n-2*k,k)/(n-3*k)!.
a(n) = n! * sum(k=0, n\3, binomial(n-2*k,k)/(n-3*k)!);
for(n=0, 21, print1(a(n),", "))
for(n=0, 100, print1((a(n)-1)%6,", "))

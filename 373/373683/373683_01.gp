\\ a(n) = n! * Sum_{k=0..floor(n/2)} binomial(n-k,k)/(n-2*k)!.
a(n) = n! * sum(k=0, n\2, binomial(n-k,k)/(n-2*k)!);
for(n=0, 21, print1(a(n),", "))
for(n=0, 100, print1((a(n)-1)%2,", "))

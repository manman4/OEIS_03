\\ a(n) = n! * Sum_{k=0..floor(2*n/9)} binomial(2*n-8*k,k)/(n-4*k)!.
a(n) = n! * sum(k=0, 2*n\9, binomial(2*n-8*k,k)/(n-4*k)!);
for(n=0, 22, print1(a(n),", "))
for(n=0, 100, print1((a(n)-1)%240,", "))

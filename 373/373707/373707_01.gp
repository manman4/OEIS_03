\\ a(n) = n! * Sum_{k=0..floor(2*n/7)} binomial(2*n-6*k,k)/(n-3*k)!.
a(n) = n! * sum(k=0, 2*n\7, binomial(2*n-6*k,k)/(n-3*k)!);
for(n=0, 21, print1(a(n),", "))
for(n=0, 100, print1((a(n)-1)%48,", "))

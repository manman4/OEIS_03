\\ a(n) = (-1)^n * n! * Sum_{k=0..floor(n/3)} binomial(k-1,n-3*k)/k!.
a(n) = (-1)^n * n! * sum(k=0, n\3, binomial(k-1, n-3*k)/k!);
for(n=0, 22, print1(a(n),", ")) 


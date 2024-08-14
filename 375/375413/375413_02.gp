\\ a(n) = (-1)^n * n! * Sum_{k=0..floor(n/2)} binomial(2*k-1,n-2*k)/k!.
a(n) = (-1)^n * n! * sum(k=0, n\2, binomial(2*k-1, n-2*k)/k!);
for(n=0, 22, print1(a(n),", ")) 


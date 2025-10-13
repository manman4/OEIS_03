\\ a(n) = n! * Sum_{k=0..floor(n/2)} binomial(3*k+1,n-2*k)/k!.
a(n) = n! * sum(k=0, n\2, binomial(3*k+1,n-2*k)/k!);
for(n=0, 21, print1(a(n),", "));   



\\ a(n) = n! * Sum_{k=0..floor(n/2)} 2^(n-3*k) * binomial(n-1-3*k/2,n-2*k)/k!.
a(n) = n! * sum(k=0, n\2, 2^(n-3*k) * binomial(n-1-3*k/2,n-2*k)/k!);
for(n=0, 19, print1(a(n), ", "));   


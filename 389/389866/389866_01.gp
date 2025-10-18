\\ a(n) = n! * Sum_{k=0..n} (k+1)^(k-1) * binomial(n+3*k,n-k)/k!.
a(n) = n! * sum(k=0, n, (k+1)^(k-1) * binomial(n+3*k,n-k)/k!);
for(n=0, 18, print1(a(n),", "))   



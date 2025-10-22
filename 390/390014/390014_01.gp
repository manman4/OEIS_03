\\ a(n) = n! * Sum_{k=0..floor(n/4)} (-1)^k * (n-3*k+1)^(n-3*k-1) * binomial(n-3*k,k)/(n-3*k)!.
a(n) = n! * sum(k=0, n\4, (-1)^k * (n-3*k+1)^(n-3*k-1) * binomial(n-3*k,k)/(n-3*k)!);
for(n=0, 20, print1(a(n),", "));   



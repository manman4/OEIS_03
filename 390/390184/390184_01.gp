\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k+1)^(n-3*k-1) * binomial(2*n-5*k-1,k)/(n-3*k)!.
a(n) = n! * sum(k=0, n\3, (n-3*k+1)^(n-3*k-1) * binomial(2*n-5*k-1,k)/(n-3*k)!);
for(n=0, 19, print1(a(n),", "));   



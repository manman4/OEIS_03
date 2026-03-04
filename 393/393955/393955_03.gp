\\ a(0) = 1; a(n) = (n-1)! * Sum_{k=0..floor(n/2)} n^k * (2*n)^(n-2*k) * binomial(n-k,k)/(n-k)!.
a(n) = if(n==0, 1, (n-1)! * sum(k=0, n\2, n^k * (2*n)^(n-2*k) * binomial(n-k,k)/(n-k)!));
for(n=0, 18, print1(a(n),", "));
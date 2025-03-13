M=18;

\\ a(n) = (n-1)! * Sum_{k=0..n-1} (k+1)^(n-k-1) * binomial(2*n,k)/(n-k-1)! for n > 0.
a(n) = if(n==0, 1, (n-1)! * sum(k=0, n-1, (k+1)^(n-k-1) * binomial(2*n,k)/(n-k-1)!));
for(n=0, M, print1(a(n),", "));


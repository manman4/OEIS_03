M=17;

\\ a(n) = (n-1)! * Sum_{k=0..n-1} (n+1)^(n-k-1) * binomial(3*n+k-1,k)/(n-k-1)! for n > 0.
a(n) = if(n==0, 1, (n-1)! * sum(k=0, n-1, (n+1)^(n-k-1) * binomial(3*n+k-1,k)/(n-k-1)!));
for(n=0, M, print1(a(n),", "));


\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * binomial(2*k+1,k)/( (2*k+1)*(n-k)! )
a295238(n) = n! * sum(k=0, n, k^(n-k) * binomial(2*k+1, k)/( (2*k+1)*(n-k)! ));

\\ a(n) = A295238(n+1)/(n+1).
a(n) = a295238(n+1)/(n+1);
for(n=0, 15, print1(a(n),", "));


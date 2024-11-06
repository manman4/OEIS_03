\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * binomial(3*k+1,k)/( (3*k+1)*(n-k)! )
A364983(n) = n! * sum(k=0, n, k^(n-k) * binomial(3*k+1,k)/( (3*k+1)*(n-k)! ));

\\ a(n) = A364983(n+1)/(n+1).
a(n) = A364983(n+1)/(n+1);
for(n=0, 15, print1(a(n),", "));


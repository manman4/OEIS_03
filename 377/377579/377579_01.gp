M=18;

\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * binomial(4*n-4*k+4,k)/( (n-k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, k^(n-k) * binomial(4*n-4*k+4,k)/( (n-k+1)*(n-k)! ));
for(n=0, M, print1(a(n), ", ")) 
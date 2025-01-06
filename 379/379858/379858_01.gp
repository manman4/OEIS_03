M=18;

\\ a(n) = n! * Sum_{k=0..n} (-3*n+4*k-1)^(n-k) * binomial(3*n-3*k+1,k)/( (3*n-3*k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, (-3*n+4*k-1)^(n-k) * binomial(3*n-3*k+1,k)/((3*n-3*k+1)*(n-k)!));
for(n=0, M, print1(a(n),", "));


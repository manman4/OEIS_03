M=20;

\\ a(n) = n! * Sum_{k=0..n} (-n+2*k-1)^(n-k) * binomial(n-k+1,k)/( (n-k+1)*(n-k)! ).
a(n) = n! * sum(k=0, n, (-n+2*k-1)^(n-k) * binomial(n-k+1,k)/((n-k+1)*(n-k)!));
for(n=0, M, print1(a(n),", "));


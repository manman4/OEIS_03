M=17;

\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * binomial(4*k+2,k)/( (k+1)*(n-k)! ).
a(n) = sum(k=0, n, k^(n-k) * binomial(4*k+2,k)/(k+1) * n!/(n-k)!);
for(n=0, M, print1(a(n), ", ")) 
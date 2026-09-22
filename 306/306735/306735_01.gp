\\ A306680(n,k) = Sum_{j=0..n} binomial(n-j,k*j).
a306680(n, k) = sum(j=0, n, binomial(n-j, k*j));
\\ A(n,k) = (k+1)*A306680(n,k) - A306680(n-1,k) for n > 0.
a(n, k) = if(n==0, k+1, (k+1)*a306680(n, k) - a306680(n-1, k));
for(n=0, 12, for(k=0, n, print1(a(k, n-k), ", ")));


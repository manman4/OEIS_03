a007559(n) = prod(k=0, n-1, 3*k+1);
for(n=0, 20, print1(a007559(n), ", "))

\\ a(0) = 1; a(n) = -Sum_{k=1..n} A007559(k) * binomial(n-1,k-1) * a(n-k).
a(n) = if(n==0, 1, -sum(k=1, n, a007559(k) * binomial(n-1, k-1) * a(n-k)));
for(n=0, 20, print1(a(n), ", "))

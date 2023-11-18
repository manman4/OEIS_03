M=17;

\\ a(0) = 1; a(n) = a(n-1) - 2*Sum_{k=1..n-1} (-4)^k * binomial(n-1,k) * a(n-k).
a(n) = if(n==0, 1, a(n-1) - 2*sum(k=1, n-1, (-4)^k * binomial(n-1,k) * a(n-k)));
for(n=0, M, print1(a(n), ", "));
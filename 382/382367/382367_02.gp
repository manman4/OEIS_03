M=25;

b(n) = if(n<1, 0, 1+valuation(n, 3));
for(n=1, 100, print1(b(n), ", "));

\\ a(0) = 1; a(n) = Sum_{k=1..n} A051064(k) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, b(k)*a(n-k)));
for(n=0, M, print1(a(n), ", "));



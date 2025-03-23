M=25;

b(n) = if(n<0, 0, 1+valuation(n+1, 4));
for(n=0, 100, print1(b(n), ", "));

\\ a(0) = 1; a(n) = Sum_{k=1..n} A115362(k-1) * a(n-k).
a(n) = if(n==0, 1, sum(k=1, n, b(k-1)*a(n-k)));
for(n=0, M, print1(a(n), ", "));



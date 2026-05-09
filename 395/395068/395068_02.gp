\\ a(n) = Sum_{1 <= i < j <= n} ( n!/(i*j) )^4.
a(n) = sum(i=1, n-1, sum(j=i+1, n, (n!/i/j)^4));
for(n=0, 30, print1(a(n),", "));

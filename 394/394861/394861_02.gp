\\ a(n) = Sum_{1<= i < j < k <= n} ( n!/(i*j*k) )^3.
a(n) = sum(i=1, n-1, sum(j=i+1, n, sum(k=j+1, n, (n!/i/j/k)^3)));
for(n=0, 30, print1(a(n),", "));

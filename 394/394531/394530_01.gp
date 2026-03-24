M=15;

\\ a(n) = n^(2*(n-1)) - Sum_{k=1..n-1} n^(2*(n-1-k)) * k^2 * a(k).
a(n) = n^(2*(n-1))-sum(k=1, n-1, n^(2*(n-1-k))*k^2*a(k));
for(n=1, M, print1(a(n),", "));

\\ Sum_{k=1..n} k^2 * a(k)/n^(2*k) = 1.
for(n=1, 20, print1(sum(k=1, n, k^2*a(k)/n^(2*k)), ", "));


M=21;

\\ a(0) = 1; a(n) = 2 * Sum_{k=0..n-1} (5-3*k/n) * (n-k) * a(k).
a(n) = if(n==0, 1, 2*sum(k=0, n-1, (5-3*k/n)*(n-k)*a(k)));
for(n=0, M, print1(a(n),", "))

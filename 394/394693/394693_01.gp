\\ a(n) = 2 * (n!)^8 * Sum_{k=1..n} (-1)^(k-1)/(k^6 * (n-k)! * (n+k)!).
a(n) = 2 * (n!)^8 * sum(k=1, n, (-1)^(k-1)/(k^6 * (n-k)! * (n+k)!));
for(n=0, 25, print1(a(n), ", "));

\\ a(n) = (n!)^6 * Sum_{1<=i<=j<=k<=n} 1/(i*j*k)^2.
b(n) = (n!)^6 * sum(i=1, n, sum(j=i, n, sum(k=j, n, 1/(i^2 * j^2 * k^2))));
for(n=0, 100, print1(a(n)-b(n), ", "));
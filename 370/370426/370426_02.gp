M=24;

\\ a(n) = (1/2) * Sum_{k=0..floor(n/2)-2} (-1)^k * (n-k-2)! / k!.
b(n) = (1/2) * sum(k=0, n\2-2, (-1)^k * (n-k-2)! / k!);
for(n=0, M, print1(b(n), ", "))

a(n, k=2, q=2) = sum(j=0, n\q-k, (-1)^j*(n-(q-1)*(j+k))!/j!)/k!;
for(n=0, M, print1(a(n)-b(n), ", "))
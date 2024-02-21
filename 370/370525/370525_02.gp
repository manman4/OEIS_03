M=24;

\\ a(n) = Sum_{k=0..floor(n/3)-1} (-1)^k * (n-2*k-2)! / k!.
b(n) = sum(k=0, n\3-1, (-1)^k * (n-2*k-2)! / k!);
for(n=0, M, print1(b(n), ", "))

a(n, k=1, q=3) = sum(j=0, n\q-k, (-1)^j*(n-(q-1)*(j+k))!/j!)/k!;
for(n=0, M, print1(a(n)-b(n), ", "))
M=30;

\\ a(n) = (1/6) * Sum_{k=0..floor(n/4)-3} (-1)^k * (n-3*k-9)! / k!.
b(n) = (1/6) * sum(k=0, n\4-3, (-1)^k * (n-3*k-9)! / k!);
for(n=0, M, print1(b(n), ", "))

a(n, k=3, q=4) = sum(j=0, n\q-k, (-1)^j*(n-(q-1)*(j+k))!/j!)/k!;
for(n=0, M, print1(a(n)-b(n), ", "))
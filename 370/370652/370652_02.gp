M=27;

\\ a(n) = (1/2) * Sum_{k=0..floor(n/4)-2} (-1)^k * (n-3*k-6)! / k!.
b(n) = (1/2) * sum(k=0, n\4-2, (-1)^k * (n-3*k-6)! / k!);
for(n=0, M, print1(b(n), ", "))

a(n, k=2, q=4) = sum(j=0, n\q-k, (-1)^j*(n-(q-1)*(j+k))!/j!)/k!;
for(n=0, M, print1(a(n)-b(n), ", "))
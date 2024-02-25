M=25;

\\ a(n) = Sum_{k=0..floor(n/4)-1} (-1)^k * (n-3*k-3)! / k!.
b(n) = sum(k=0, n\4-1, (-1)^k*(n-3*k-3)!/k!);
for(n=0, M, print1(b(n), ", "))

a(n, k=1, q=4) = sum(j=0, n\q-k, (-1)^j*(n-(q-1)*(j+k))!/j!)/k!;
for(n=0, M, print1(a(n)-b(n), ", "))
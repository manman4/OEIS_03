M=11;

\\ T(n,k) = Sum_{j=0..floor(n/k)-1} (-1)^j * (n-(k-1)*(j+1))! / j!.
T(n, k) = sum(j=0, n\k-1, (-1)^j * (n-(k-1)*(j+1))!/j!);
for(n=1, M, for(k=1, n, print1(T(n,k),", ")))
\\ A(0,k) = 1 and A(n,k) = 3*k*(k+1) * A(n-1,k+2) + k^2 * A(n-1,k) for n > 0.
a(n, k) = if(n==0, 1, 3*k*(k+1)*a(n-1, k+2)+k^2*a(n-1, k));
\\ matrix(7, 7, n, k, a(n-1, k-1))

for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));

for(n=0, 15, print1(a(n, 1), ", "));
for(n=0, 15, print1(a(n, 2), ", "));


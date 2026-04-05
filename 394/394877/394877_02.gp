M=14;

\\ a(0) = -1, a(1) = 0; a(n) = (n-1)/2 * a(n-1) - a(n-2) - ((n-1)/2 * b(n-1) - b(n-2)), where b(n) = Sum_{k=1..n} a(k)*a(n-k).
b(n) = sum(k=1, n, a(k)*a(n-k));
a(n) = my(v=[-1, 0]); if(n<2, v[n+1], (n-1)/2 * a(n-1) - a(n-2) - ((n-1)/2 * b(n-1) - b(n-2)));
for(n=0, M, print1(a(n), ", "));  
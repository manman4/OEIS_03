M=9;

\\ A(n, k) = Sum_{j=0..k} j! * abs(S1(n+1, j+1)) * S2(k+1, j+1) (see Theorem 14 in Bényi and Ramírez).
a(n, k) = sum(j=0, n, j!*abs(stirling(n+1, j+1, 1))*stirling(k+1, j+1, 2));
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));

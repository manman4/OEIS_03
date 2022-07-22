M=11;

T(n, k) = n!/(k!*(n\k)!);
for(n=1, M, for(k=1, n, print1(T(n, k), ", ")));
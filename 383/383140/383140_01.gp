M=10;

T(n, k) = sum(j=k, n, 3^(n-j)*stirling(n, j, 2)*stirling(j, k, 1));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));

\\ Row sums give A133494.
for(n=0, 30, print1(sum(k=0, n, T(n, k)), ", "));

\\ Alternating row sums give A212846.
for(n=0, 21, print1(sum(k=0, n, (-1)^k*T(n, k)), ", "));


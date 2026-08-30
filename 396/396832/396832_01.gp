M=20;

\\ a(n) = Product_{k=1..n} (n - k + 2 - ceiling((n-1)/k)) unless n = 1.

a(n) = if(n==1, 1, prod(k=1, n, n-k+2-ceil((n-1)/k)));
for(n=0, M, print1(a(n),", "));

\\ Number of permutations p of [n] such that k * p(k) >= n-1 for 1 <= k <= n.
b(n) = matpermanent(matrix(n, n, i, j, i*j >= n-1));
for(n=0, M, print1(a(n)-b(n), ", "));
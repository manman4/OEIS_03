\\ T(n,k) = T(n-1,k) + n^4 * T(n-1,k-1).
T(n, k) = if(n*k==0, n^k, T(n-1, k)+n^4*T(n-1, k-1));
for(n=0, 10, for(k=0, n, print1(T(n, k),", ")));

\\ T(n,k) = [x^k] Product_{j=0..n} (1 + j^4*x).
S(n, k) = polcoef(prod(j=0, n, 1+j^4*x), k);
for(n=0, 10, for(k=0, n, print1(T(n, k)-S(n, k),", ")));
\\ T(n,0) = delta(n,1); T(n,k) = Sum_{j=k..n-1} j^(n-j) * binomial(n-1,j-1) * T(j,k-1) for n>=2 and 1<=k<=n-1.
T(n, k) = if(k==0, n==1, sum(j=k, n-1, j^(n-j)*binomial(n-1, j-1)*T(j, k-1)));
for(n=1, 9, for(k=0, n-1, print1(T(n, k),", ")));

a080108(n) = sum(k=1, n, k^(n-k)*binomial(n-1, k-1));
\\ a(n) = Sum_{k=1..n} binomial(n,k)*k^(n-k+1)
a185298(n) = sum(k=1, n, binomial(n, k)*k^(n-k+1));
a001044(n) = (n!)^2
a292062(n) = n! * (n*n! - abs(stirling(n+1, 2, 1))) / 2;
a000272(n) = n^(n-2);

\\ T(n,2) = A080108(n) - 2 = A185298(n)/n - 2 for n>=3.
for(n=1, 15, if(T(n,2)-a080108(n)+2 != 0, print("Error at n=", n)));
for(n=1, 15, if(T(n,2)-a185298(n)/n+2 != 0, print("Error at n=", n)));
\\ T(n,n-1) = A001044(n-1)
for(n=1, 15, if(T(n,n-1)-a001044(n-1) != 0, print("Error at n=", n)));
\\ T(n+1,n-1) = A292062(n).
for(n=1, 15, if(T(n+1,n-1)-a292062(n) != 0, print("Error at n=", n)));
\\ Sum_{k=0..n-1} (-1)^(n-1-k) * T(n,k) = A000272(n).
for(n=1, 15, if(sum(k=0, n-1, (-1)^(n-1-k)*T(n,k))-a000272(n) != 0, print("Error at n=", n)));
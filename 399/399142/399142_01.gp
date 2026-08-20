\\ T(n,0) = delta(n,1); T(n,k) = Sum_{j=k..n-1} j^(n-j) * binomial(n-1,j-1) * T(j,k-1) for n>=2 and 1<=k<=n-1.
T(n, k) = if(k==0, n==1, sum(j=k, n-1, j^(n-j)*binomial(n-1, j-1)*T(j, k-1)));
for(n=1, 9, for(k=0, n-1, print1(T(n, k),", ")));

a080108(n) = sum(k=1, n, k^(n-k)*binomial(n-1, k-1));
\\ a(n) = Sum_{k=1..n} binomial(n,k)*k^(n-k+1)
a185298(n) = sum(k=1, n, binomial(n, k)*k^(n-k+1));
a001044(n) = (n!)^2
a292062(n) = n! * (n*n! - abs(stirling(n+1, 2, 1))) / 2;
a000272(n) = n^(n-2);
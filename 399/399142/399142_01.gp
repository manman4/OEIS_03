\\ T(n,0) = delta(n,1); T(n,k) = Sum_{j=k..n-1} j^(n-j) * binomial(n-1,j-1) * T(j,k-1) for n>=2 and 1<=k<=n-1.
T(n, k) = if(k==0, n==1, sum(j=k, n-1, j^(n-j)*binomial(n-1, j-1)*T(j, k-1)));
for(n=1, 9, for(k=0, n-1, print1(T(n, k),", ")));


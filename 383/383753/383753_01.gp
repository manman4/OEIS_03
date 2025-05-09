\\ T(n,k) = 2^(n-k) * T(n-1,k-1) + 3^k * T(n-1,k) with T(n,k) = n^k if n*k=0.
T(n, k) = if(n*k==0, n^k, 2^(n-k)*T(n-1, k-1)+3^k*T(n-1, k));
for(n=0, 9, for(k=0, n, print1(T(n, k),", ")));
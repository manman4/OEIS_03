M=10;

T(n, k) = if(n<2, n, 2 * k * T(n-1,k) - (k-1) * k * T(n-2,k));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));

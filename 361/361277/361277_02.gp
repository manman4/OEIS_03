M=10;

T(n, k) = if(n==0, 1, (n-1)! * sum(j=1, n, j * binomial(k,j-1) * T(n-j,k)/(n-j)!));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));
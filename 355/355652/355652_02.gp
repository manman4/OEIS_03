M=11;

T(n, k) = if(n==0, 1, n!/k!*sum(j=k+1, n, 1/(j-k) * T(n-j,k)/(n-j)!));
for(n=0, M,for(k=0, n,print1(T(k, n-k), ", "))) 
M=11;

T(n, k) = if(n==0, 1, binomial(n,k)*sum(j=k+1, n, binomial(n-k, j-k)*T(n-j, k)));
for(n=0, M,for(k=0, n,print1(T(k, n-k), ", "))) 
M=10;

T(n, k) = if(k==0, n==1, polcoef(prod(j=1, n, 1/(1-x^j+x*O(x^(n^k-n)))), n^k-n));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));
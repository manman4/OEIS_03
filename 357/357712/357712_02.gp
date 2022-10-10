M=11;

T(n, k) = round((prod(j=0, n-1, sqrt(k)+j)+prod(j=0, n-1, -sqrt(k)+j)))/2;
for(n=0, M, for(k=0, n, print1(T(k,n-k),", ")))   
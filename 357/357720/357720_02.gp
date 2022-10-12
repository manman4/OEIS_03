M=11;

T(n, k) = (-1)^n*round((prod(j=0, n-1, sqrt(k)*I+j)+prod(j=0, n-1, -sqrt(k)*I+j)))/2;
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
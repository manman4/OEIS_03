M=11;

T(n, k) = if(n<2, 1, T(n-1,k) - k*(n-1)*T(n-2,k));
for(n=0, M, for(k=0, n, print1(T(k,n-k), ", "))); 
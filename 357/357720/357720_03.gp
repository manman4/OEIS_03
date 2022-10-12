M=11;

T(n, k) = if(n<2, 0^n, -(2*n-3)*T(n-1, k)-(n^2-4*n+4+k)*T(n-2, k));
for(n=0, M, for(k=0, n, print1(T(k,n-k),", "))) 
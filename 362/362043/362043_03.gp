M=11;

T(n, k) = if(n<3, 1, T(n-1,k) + k * binomial(n-1,2) * T(n-3,k));
for(n=0, M, for(k=0, n, print1(T(k,n-k), ", "))); 
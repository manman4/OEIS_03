M=9;

T(n, k) = if(k==0, 0^n, my(A=1); for(i=1, n, A=( 1 + x * A^(3/k) * (1 + A^(3/k)) )^k + x*O(x^n) ); polcoeff(A, n));

for(n=0, M, for(k=0, n , print1(T(k,n-k), ", ")));

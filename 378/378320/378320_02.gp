M=11;

\\ G.f. A_k(x) of column k satisfies A_k(x) = ( 1 + x + x^2 * A_k(x)^(2/k) )^k for k > 0.
T(n, k) = if(k==0, 0^n, my(A=1); for(i=1, n, A=( 1 + x + x^2 * A^(2/k) )^k + x*O(x^n) ); polcoeff(A, n));

for(n=0, M, for(k=0, n , print1(T(k,n-k), ", ")));

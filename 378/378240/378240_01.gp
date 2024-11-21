M=9;

T(n, k, t=3, u=3) = if(k==0, 0^n, k*sum(r=0, n, binomial(n, r)*binomial(t*n+u*r+k, n)/(t*n+u*r+k)));
matrix(7, 7, n, k, T(n-1, k-1))

for(n=0, M, for(k=0, n , print1(T(k,n-k), ", ")));

\\ T(n,0) = 0^n and T(n,k) = k * Sum_{r=0..n} binomial(n,r) * binomial(3*n+3*r+k,n)/(3*n+3*r+k) for k > 0.
S(n, k) = if(k==0, 0^n, k*sum(r=0, n, binomial(n, r)*binomial(3*n+3*r+k, n)/(3*n+3*r+k)));
for(n=0, M, for(k=0, n, print1(S(k,n-k) - T(k,n-k), ", ")));
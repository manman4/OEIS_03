M=20;

\\ T(n,k) = Sum_{j=k..n} Stirling1(n,j) * Stirling1(j,k).
T(n, k) = sum(j=k, n, stirling(n,j,1) * stirling(j,k,1));
for(n=0, M, for(k=0, n, print1(T(n, k),", ")));

\\ E.g.f. of column k (with leading zeros): f(x)^k / k! with f(x) = log(1 + log(1 + x)). 
S(n, k) = my(N=30, x='x+O('x^(n+1))); n! * polcoef( log(1 + log(1 + x))^k /k!, n);
for(n=0, M, for(k=0, n, print1(T(n, k)-S(n, k),", ")));



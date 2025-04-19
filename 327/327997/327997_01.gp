M=9;

\\ T(n,k) = Sum_{j=k..n} 3^(n-j) * binomial(n,j) * |Stirling1(j,k)|.
T(n, k) = sum(j=k, n, 3^(n-j)*binomial(n, j)*abs(stirling(j, k, 1)));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));

\\ E.g.f. of column k (with leading zeros): (exp(f(x)) - 1)^k / k! with f(x) = (exp(2*x) - 1)/2.
S(n, k) = my(N=30, x='x+O('x^(n+1)), f(x) = (exp(2*x) - 1)/2); n! * polcoef( ( exp(f(x)) - 1 )^k / k!, n);
for(n=0, 20, for(k=0, n, print1(T(n, k)-S(n, k),", ")));

\\ Row sums give A380228.
for(n=0, 21, print1(sum(k=0, n, T(n, k)), ", "));




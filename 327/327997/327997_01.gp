M=9;

\\ T(n,k) = Sum_{j=k..n} 3^(n-j) * binomial(n,j) * |Stirling1(j,k)|.
T(n, k) = sum(j=k, n, 3^(n-j)*binomial(n, j)*abs(stirling(j, k, 1)));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));

\\ E.g.f. of column k (with leading zeros): (-log(1 - x))^k * exp(3*x) / k!.
S(n, k) = my(N=30, x='x+O('x^(n+1))); n! * polcoef( (-log(1 - x))^k * exp(3*x) / k!, n);
for(n=0, 30, for(k=0, n, print1(T(n, k)-S(n, k),", ")));

\\ Row sums in A053486.
for(n=0, 20, print1(sum(k=0, n, T(n, k)), ", "));




M=10;

\\ T(n,k) = Sum_{j=k..n} 3^(n-j) * |Stirling1(n,j)| * Stirling2(j,k).
T(n, k) = sum(j=k, n, 3^(n-j)*abs(stirling(n, j, 1))*stirling(j, k, 2));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));

\\ E.g.f. of column k (with leading zeros): (1/(1 - 3*x)^(1/3) - 1)^k / k!.
S(n, k) = my(N=30, x='x+O('x^(n+1))); n! * polcoef( (1/(1 - 3*x)^(1/3) - 1)^k /k!, n);
for(n=0, 20, for(k=0, n, print1(T(n, k)-S(n, k),", ")));




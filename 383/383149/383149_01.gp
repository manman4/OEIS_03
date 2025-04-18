M=9;

\\ T(n,k) = Sum_{j=k..n} 2^(n-j) * Stirling2(n,j) * |Stirling1(j,k)|.
T(n, k) = sum(j=k, n, 2^(n-j) * stirling(n, j, 2) * abs(stirling(j, k, 1)));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));

\\ E.g.f. of column k (with leading zeros): g(x)^k / k! with g(x) = -log(1 - (exp(2*x) - 1)/2).
S(n, k) = my(N=30, x='x+O('x^(n+1))); n! * polcoef( ( -log(1 - (exp(2*x) - 1)/2) )^k /k!, n);
for(n=0, 20, for(k=0, n, print1(T(n, k)-S(n, k),", ")));

\\ Row sums give A122704.
for(n=0, 20, print1(sum(k=0, n, T(n, k)), ", "));

\\ Sum_{k=0..n} (-1)^k * T(n,k) = f_m(1) = -2^(n-1) for n > 0.
for(n=0, 21, print1(sum(k=0, n, (-1)^k*T(n, k)), ", "));


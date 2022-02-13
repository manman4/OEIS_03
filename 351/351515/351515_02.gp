M=19;

T(n, k) = if(k==0, n<=1, sum(j=0, n, stirling(n, j, 2)*T(j, k-1)));
a(n) = sum(k=1, n-1, binomial(n-1, k)*T(k, 5)*T(n-k, 5));
for(n=2, M, print1(a(n), ", "));
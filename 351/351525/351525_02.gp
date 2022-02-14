M=19;

T(n, k) = if(k==0, n==1, sum(j=0, n, abs(stirling(n, j, 1))*T(j, k-1)));
a(n) = (-1)^n*sum(k=1, n-1, binomial(n-1, k)*T(k, 3)*T(n-k, 3));
for(n=2, M, print1(a(n), ", "));
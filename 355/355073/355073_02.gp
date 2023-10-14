M=13;

T(n, k) = if(k==1, 1, sum(j=1, n-1, 3^(j*(n-j))*binomial(n-1, j)*T(j, k-1)));
a(n) = if(n==0, 1, sum(k=1, n, T(n, k)));
for(n=0, M, print1(a(n), ", "));
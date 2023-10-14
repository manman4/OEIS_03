M=13;

T(n, k) = if(k==1, (n-1)!, sum(j=1, n, stirling(n, j, 1)*T(j, k-1)));
a(n) = T(n, n);
for(n=1, M, print1(a(n), ", "));
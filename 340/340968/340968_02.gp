T(n, k) = 1+k*sum(j=0, n-1, T(j, k)*T(n-1-j, k));
for(n=0, 9, for(k=0, n, print1(T(k, n-k), ", ")))
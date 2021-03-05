T(n, k) = round(cos(n*acos(k)));
for(n=0, 10, for(k=0, n, print1(T(k, n-k), ", ")))
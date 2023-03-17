M=9;

T(n, k) = sum(j=0, n, (n+(k-1)*j)!/(k*j)! * binomial(n,j));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));

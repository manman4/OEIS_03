M=9;

T(n, k) = if(k==0, 1, sum(j=0, n, (n+(k-1)*(j+1))!/(k*j+k-1)! * binomial(n,j)));
for(n=0, M, for(k=0, n, print1(T(k, n-k), ", ")));

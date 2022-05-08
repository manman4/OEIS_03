M=10;

T(n, k) = if(k==0, 0^n, sum(j=0, n\k, (k*j)!*stirling(n, k*j, 2)/(k!^j*j!)));
for(n=0, M, for(k=0, n, print1(T(n, k), ", ")));
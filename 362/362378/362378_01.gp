T(n, k) = n! * sum(j=0, n\3, (k/6)^j*(j+1)^(n-2*j-1)/(j!*(n-3*j)!));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))

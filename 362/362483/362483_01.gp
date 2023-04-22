T(n, k) = n! * sum(j=0, n\2, (k/2)^j*(2*j+1)^(n-j-1)/(j!*(n-2*j)!));
for(n=0, 10, for(k=0, n , print1(T(k,n-k), ", ")))

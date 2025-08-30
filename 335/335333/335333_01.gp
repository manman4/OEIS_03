T(n, k) = pollegendre(n, 2*k+1);
for(n=0, 10, for(k=0, n, print1(T(k,n-k),", ")));

\\ A(n,k) = [x^n] (1+x)^(k*n+1) * (2+x)^n.
a(n, k) = polcoef( (1+x)^(k*n+1) * (2+x)^n + x*O(x^n), n);
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));


   
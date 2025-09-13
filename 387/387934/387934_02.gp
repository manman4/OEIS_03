\\ A(n,k) = [x^n] (1+x)^n/(1-x)^(k*n+1).
a(n, k) = polcoef( (1+x)^n/(1-x + x*O(x^n))^(k*n+1), n);
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));


   
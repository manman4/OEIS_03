\\ A(n,k) = [x^n] (1 + 2*x + (k^2+1)*x^2)^n.
a(n,k) = polcoef((1 + 2*x + (k^2+1)*x^2)^n, n);
for(n=0, 10, for(k=0, n, print1(a(k,n-k),", ")));

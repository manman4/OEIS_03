\\ A(n,k) = [x^n] ( (1+2*x) * (1+x)^k )^n.
a(n,k) = polcoef( ( (1+2*x) * (1+x)^k + x*O(x^n) )^n, n); 
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));



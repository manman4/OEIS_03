\\ a(n) = [x^n] 1/(1 - n*x) * Product_{k=0..n-1} (1 + k*x)/(1 - k*x).
a(n) = polcoef(1/(1 - n*x) * prod(k=0, n-1, (1 + k*x)/(1 - k*x) + x*O(x^n)), n);
for(n=0, 20, print1(a(n),", "))                   


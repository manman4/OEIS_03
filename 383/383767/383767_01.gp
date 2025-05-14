\\ a(n) = [x^n] Product_{k=0..n-1} (1 + k*x)/(1 - k*x).
a(n) = polcoef(prod(k=0, n-1, (1 + k*x)/(1 - k*x) + x*O(x^n)), n);
for(n=0, 18, print1(a(n),", "))                   


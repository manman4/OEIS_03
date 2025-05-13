\\ a(n) = [x^n] 1/Product_{k=0..n-1} (1 - k*x)^2.
a(n) = polcoef(1/prod(k=0, n-1, (1-k*x)^2 + x*O(x^n)), n);
for(n=0, 17, print1(a(n),", "))                   


\\ a(n) = A129256(2*n) = [x^(2*n)] Product_{k=0..2*n} (1 + k*x)^2.
a(n) = polcoef(prod(k=0, 2*n, (1 + k*x)^2 + x*O(x^(2*n))), 2*n);  
for(n=0, 20, print1(a(n),", "));

\\ a(n) = [x^n] Product_{k=0..n-1} (1 + k*x)^2. 
a(n) = polcoef(prod(k=0, n-1, (1+k*x)^2), n);
for(n=0, 20, print1(a(n),", "))                   


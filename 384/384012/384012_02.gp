M=17;

\\ a(n) = [x^n] Product_{k=0..n} (1 + k*x)^3.
a(n) = polcoef(prod(k=0, n, (1 + k*x)^3 + x*O(x^n)), n); 
for(n=0, M, print1(a(n),", "));

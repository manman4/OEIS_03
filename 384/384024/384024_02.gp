M=18;

\\ a(n) = [x^n] Product_{k=0..n} (1 + (n+k)*x).
a(n) = polcoef(prod(k=0, n, (1 + (n+k)*x) + x*O(x^(n))), n); 
for(n=0, M, print1(a(n),", "));



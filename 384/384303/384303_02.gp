M=17;

\\ a(n) = Product_{k=0..2*n-1} (3*n+k+1).
a(n) = prod(k=0, 2*n-1, 3*n+k+1);
for(n=0, M, print1(a(n),", "));

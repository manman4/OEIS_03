M=25;

a003520(n) = polcoef((1/(1 - x - x^5) + O(x^(n+1))), n);
for(n=0, M,print1(a003520(n),", "))

\\ a(n) = A003520(5*n).
a(n) = a003520(5*n);
for(n=0, M,print1(a(n),", "))

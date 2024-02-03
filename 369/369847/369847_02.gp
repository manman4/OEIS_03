M=31;

a052920(n) = polcoef((1/(1 - x^3 - x^5) + O(x^(n+1))), n);
for(n=0, M,print1(a052920(n),", "))

\\ a(n) = A052920(5*n-2).
a(n) = a052920(5*n-2);
for(n=1, M, print1(a(n),", "))

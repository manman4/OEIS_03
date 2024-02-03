M=26;

a001687(n) = polcoef((x/(1 - x^2 - x^5) + O(x^(n+1))), n);
for(n=0, M,print1(a001687(n),", "))

\\ a(n) = A001687(5*n-3).
a(n) = a001687(5*n-3);
for(n=1, M,print1(a(n),", "))

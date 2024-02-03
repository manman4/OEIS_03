M=34;

a017827(n) = polcoef((1/(1 - x^4 - x^5) + O(x^(n+1))), n);
for(n=0, M, print1(a017827(n),", "))

\\ a(n) = A017827(5*n-2).
a(n) = a017827(5*n-2);
for(n=1, M, print1(a(n),", "))

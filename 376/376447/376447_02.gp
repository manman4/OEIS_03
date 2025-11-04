M=20;

a390387(n) = sum(k=0, n, binomial(n+7*k, k));
a390391(n) = sum(k=0, n, binomial(n+7*k+7, k));

\\ a(n) = A390387(n) - 8*A390391(n-1).
a(n) = a390387(n) - 8*a390391(n-1);
for(n=0, M, print1(a(n),", "));

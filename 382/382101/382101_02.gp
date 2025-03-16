M=10;

\\ Square array A(n,k), n >= 0, k >= 0, read by antidiagonals downwards, where column k is the expansion of e.g.f. exp(B_k(x) - 1), where B_k(x) = 1 + x*B_k(x)^k.
a(n, k) = n! * polcoef(exp( sum(j=0, n, binomial(k*j+1, j)/(k*j+1)*x^j+x*O(x^n)) - 1), n);
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 



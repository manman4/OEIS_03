M=9;

\\ Square array A(n,k), ... where column k is the expansion of e.g.f. B(x)^k, where B(x) is the e.g.f of A140049.
a(n, k) = my(A=1); for(i=1, n, A = exp( x*A * subst(A, x, x*A + x*O(x^n)) ) ); n! * polcoef(A^k, n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", "))) 

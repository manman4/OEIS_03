M=22;

a(n, k) = my(A=1); for(i=1, n, A = 1 + x * subst(A, x, x*A^4) + x*O(x^n) ); polcoef(A^k, n);
for(n=0, M, print1(a(n, 1),", ")); 

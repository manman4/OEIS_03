M=10; 

a(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * subst(A, x, x*A) ) + x*O(x^n) ); polcoef(A^k, n);
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")))
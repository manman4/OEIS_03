M=22;

a(n, k) = my(A=1); for(i=1, n, A = 1 + x * subst(A, x, x*A^3) + x*O(x^n) ); polcoef(A^k, n);
for(n=0, M, print1(a(n, 3),", ")); 

\\ G.f. A(x) satisfies A(x) = ( 1 + x * A(x*A(x))^(1/3) )^3.
b(n) = my(B=1); for(i=1, n, B = (1 + x * subst(B, x, x*B)^(1/3) + x*O(x^n) )^3 ); polcoef(B, n);
for(n=0, M, print1(a(n, 3)-b(n),", "));

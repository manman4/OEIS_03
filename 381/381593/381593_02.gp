M=19;

a(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * A * subst(A, x, x*A) )^2 + x*O(x^n) ); Vec(A^k);
print(a(M, 1));

\\ G.f.: B(x)^2, where B(x) is the g.f. of A381600.
print(a(M, 1/2));


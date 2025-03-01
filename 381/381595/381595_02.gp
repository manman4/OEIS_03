M=18;

a(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * A * subst(A, x, x*A) )^3 + x*O(x^n) ); Vec(A^k);
print(a(M, 1));

\\ G.f.: B(x)^3, where B(x) is the g.f. of A381601.
print(a(M, 1/3));


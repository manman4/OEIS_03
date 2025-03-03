M=19;

a(n, k) = my(A=1); for(i=1, n, A = 1 + x * A^2 * subst(A, x, x*A^3)^3 + x*O(x^n) ); Vec(A^k);
print(a(M, 1));




M=21;

a(n, k) = my(A=1); for(i=1, n, A = 1/(1 - x * A^2 * subst(A, x, x*A^2)^2 ) + x*O(x^n) ); Vec(A^k);
for(k=1, 10, print(k,": ",a(M, k)));


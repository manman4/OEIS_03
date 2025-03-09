a(n, k, m) = my(A=1, C=(1 - sqrt(1 - 4*x + x*O(x^n)))/(2*x)); for(i=1, n, A=1/(1 - x*A^k)^2 * subst(C, x, x*A^m) ); Vec(A);
a(20, 0, 1)

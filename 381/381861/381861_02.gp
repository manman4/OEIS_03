a(n, k, m) = my(A=1, C=(1 - sqrt(1 - 4*x + x*O(x^n)))/(2*x)); for(i=1, n, A=(1 + x*A^k)^4 * subst(C, x, x*A^m) ); Vec(A);
a(20, 1, 0)

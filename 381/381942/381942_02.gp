a(n, k, m) = my(A=1, B=sum(k=0, n, binomial(4*k,k)/(3*k+1) * x^k) + x*O(x^n)); for(i=1, n, A=(1 + x*A^k)^3 * subst(B, x, x*A^m) ); Vec(A);
a(20, 0, 1)

M=22;

a(n) = my(A=1); for(i=1, n, A = (1 + x * subst(A, x, x*A) )^2 + x*O(x^n) ); Vec(A);
print1(a(M))


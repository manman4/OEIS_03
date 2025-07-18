\\ A(x) = 1/( (1-x) * (1-x*A(-x)) ).
my(A=1, n=42); for(i=1, n, A= 1/( (1-x) * (1-x*subst(A, x, -x)) + x*O(x^n) ) ); Vec(A)

\\ A(x)*A(-x) = B(-x^2), where B(x) is the g.f. of A064641.
my(A=1, n=42); for(i=1, n, A= 1/( (1-x) * (1-x*subst(A, x, -x)) + x*O(x^n) ) ); Vec(A * subst(A, x, -x))

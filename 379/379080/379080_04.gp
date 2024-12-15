M=23;

\\ A(x) = ( (1 + x*A(x)) * (1 + x^2*A(x)^(5/2)) )^2.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x^2*A^(5/2)) + x*O(x^n))^2); Vec(A);
seq(M)

\\ A(x) = B(x)^2 where B(x) is the g.f. of A200719.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x^2*A^(5/2)) + x*O(x^n))^2); Vec(A^(1/2));
seq(M)
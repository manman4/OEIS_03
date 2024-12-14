M=18;

\\ A(x) = ( (1 + x*A(x)) * (1 + x*A(x)^(5/4)) )^4.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x*A^(5/4)) + x*O(x^n))^4); Vec(A);
seq(M)

\\ A(x) = B(x)^4 where B(x) is the g.f. of A239108.
seq(n) = my(A=1); for(i=1, n, A=( (1 + x*A) * (1 + x*A^(5/4)) + x*O(x^n))^4); Vec(A^(1/4));
seq(M)
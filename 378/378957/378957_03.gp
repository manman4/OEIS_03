\\ A(x) = 1 + x * A(x)^2 * (1 - A(x) + A(x)^2 - A(x)^3 + A(x)^4 - A(x)^5 + A(x)^6).
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^2*(1 - A + A^2 - A^3 + A^4 - A^5 + A^6) + x*O(x^n)); Vec(A);
seq(30)

\\ A(x) = sqrt(B(x)) where B(x) is the g.f. of A366402.
seq(n) = my(A=1); for(i=1, n, A=1 + x*A^2*(1 - A + A^2 - A^3 + A^4 - A^5 + A^6) + x*O(x^n)); Vec(A^2);
seq(30)
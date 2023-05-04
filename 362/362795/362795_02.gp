M=21;

a(n) = my(A=1); for(i=1, n, A = (1 + x + x*O(x^n))^A^(x^2 + x*O(x^n)) ); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "))

a_vector(n) = my(A=1); for(i=1, n, A = (1 + x + x*O(x^n))^A^(x^2 + x*O(x^n)) ); serlaplace(A);
a_vector(M)

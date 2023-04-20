M=21;

a(n) = my(A=1); for(i=1, n, A = exp(x + x^3/2 * A +x*O(x^n) ) ); n!*polcoeff(A, n);
for(n=0, M, print1(a(n), ", "))

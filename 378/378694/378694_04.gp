\\ G.f. A(x) satisfies: A(x) = 1 - x * A(x)^6 * (1 - 2 * A(x)).
a(n) = my(A=1+x*O(x^n)); for(i=0, n, A=1-x*A^6*(1-2*A)); polcoeff(A, n);
for(n=0, 20, print1(a(n),", "))    
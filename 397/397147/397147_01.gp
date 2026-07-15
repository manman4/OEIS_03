\\ G.f. A(x), with A(0) = 0, satisfies A(x-x^3) = x + A(x)^2.
my(A=x, N=30, g=serreverse(x-x^3+x*O(x^N))); for(k=1, N, A=g+subst(A, x, g)^2); Vec(A)
N = 20;
X = 'x + O('x^(N+1));

C = (1 - sqrt(1 - 4*X))/2;

A = X;
for(i = 1, N, A = C + subst(A, 'x, C)^3);

vector(N, n, polcoef(A, n))

\\ G.f. A(x), with A(0) = 0, satisfies A(x-x^2) = x + A(x)^3.
my(A=x, N=30, g=serreverse(x-x^2+x*O(x^N))); for(k=1, N, A=g+subst(A, x, g)^3); Vec(A)

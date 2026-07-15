N = 20;
X = 'x + O('x^(N+1));
C = serreverse(X-X^3);
A = X;
for(i = 1, N, A = C + subst(A, 'x, C)^2);
vector(N, n, polcoef(A, n))
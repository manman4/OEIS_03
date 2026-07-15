N = 20;
X = 'x + O('x^(N+1));
C = (1 - sqrt(1 - 4*X))/2;
A = X;
for(i = 1, N, A = C + subst(A, 'x, C)^4);
vector(N, n, polcoef(A, n))

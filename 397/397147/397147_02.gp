a_vector(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(k=1, m-1, v[k]*v[m-k])+sum(r=1, m\3, (-1)^(r+1)*binomial(m-2*r, r)*v[m-2*r])); v;
a_vector(30)

\\ a(1) = 1; a(n) = Sum_{k=1..n-1} a(k)*a(n-k) + Sum_{r=1..floor(n/3)} (-1)^(r+1) * binomial(n-2*r,r) * a(n-2*r).
a(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(i=1, m-1, v[i]*v[m-i])+sum(r=1, m\3, (-1)^(r+1)*binomial(m-2*r, r)*v[m-2*r])); v[n];
\\ G.f. A(x), with A(0) = 0, satisfies A(x-x^3) = x + A(x)^2.
my(N=20, x='x+O('x^N), A=sum(k=1, N-1, a(k)*x^k)); Vec(subst(A, 'x, x-x^3) - (x + A^2))
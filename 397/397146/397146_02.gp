a_vector(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(i=1, m-3, sum(j=1, m-i-2, sum(k=1, m-i-j-1, v[i]*v[j]*v[k]*v[m-i-j-k])))+sum(r=1, m\2, (-1)^(r+1)*binomial(m-r, r)*v[m-r])); v;
a_vector(30)

\\ a(1) = 1; a(n) = Sum_{i, j, k, l>=1 and i+j+k+l=n} a(i)*a(j)*a(k)*a(l) + Sum_{r=1..floor(n/2)} (-1)^(r+1) * binomial(n-r,r) * a(n-r).
a(n) = my(v=vector(n)); v[1]=1; for(m=2, n, v[m]=sum(i=1, m-3, sum(j=1, m-i-2, sum(k=1, m-i-j-1, v[i]*v[j]*v[k]*v[m-i-j-k])))+sum(r=1, m\2, (-1)^(r+1)*binomial(m-r, r)*v[m-r])); v[n];
\\ G.f. A(x), with A(0) = 0, satisfies A(x-x^2) = x + A(x)^4.
my(N=20, x='x+O('x^N), A=sum(k=1, N-1, a(k)*x^k)); Vec(subst(A, 'x, x-x^2) - (x + A^4))


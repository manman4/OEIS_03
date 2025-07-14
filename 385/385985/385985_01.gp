M=16;

\\ a(0) = 1; a(n) = Sum_{k=0..n-1} (2^k + 1) * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = if(n==0, 1, sum(k=0, n-1, (2^k + 1) * binomial(n-1,k) * a(k) * a(n-1-k)));
for(n=0, 13, print1(a(n), ", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, i-1, (2^j+1)*binomial(i-1, j)*v[j+1]*v[i-j])); v;
a_vector(M)

print("------------------");

\\ E.g.f. A(x) satisfies A'(x) = A(x) * (A(2*x) + A(3*x)).
v = sum(k=0, M, a(k) * x^k/k!) + x*O(x^M);
deriv_v = deriv(v);
u = v * (subst(v, x, 2*x) + subst(v, x, 3*x));
print(deriv_v)
print(u)
print(deriv_v - u)

M=14;

\\ a(n) = 1 + Sum_{k=0..n-1} (-1)^k * binomial(n-1,k) * a(k) * a(n-1-k).
a(n) = 1 + sum(k=0, n-1, (-1)^k * binomial(n-1,k) * a(k) * a(n-1-k));
for(n=0, 15, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=1+sum(j=0, i-1, (-1)^j*binomial(i-1, j)*v[j+1]*v[i-j])); v;
a_vector(29)

print("------------------");

\\ E.g.f. A(x) satisfies A'(x) = exp(x) + A(x) * A(-x).
v = sum(k=0, M, a(k) * x^k/k!) + x*O(x^M);
deriv_v = deriv(v);
u = exp(x + x*O(x^M)) + v * subst(v, x, -x);
print(deriv_v)
print(u)
print(deriv_v - u)
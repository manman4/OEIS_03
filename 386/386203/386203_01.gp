M=27;

\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/4)} binomial(n-1,4*k) * a(4*k) * a(n-1-4*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\4, binomial(n-1,4*k) * a(4*k) * a(n-1-4*k)));
for(n=0, 20, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\4, binomial(i-1, 4*j)*v[4*j+1]*v[i-4*j])); v;
a_vector(M)

print("------------------");

\\ E.g.f. A(x) satisfies A'(x) = A(x) * (A(x) + A(i*x) + A(-x) + A(-i*x))/4, where i is the imaginary unit.
v = sum(k=0, M, a(k) * x^k/k!) + x*O(x^M);
deriv_v = deriv(v);
u = v * (v + subst(v, x, I*x) + subst(v, x, -I*x) + subst(v, x, -x))/4;
print(deriv_v)
print(u)
print(deriv_v - u)
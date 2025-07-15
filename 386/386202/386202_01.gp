M=26;

\\ a(0) = 1; a(n) = Sum_{k=0..floor((n-1)/3)} binomial(n-1,3*k) * a(3*k) * a(n-1-3*k).
a(n) = if(n==0, 1, sum(k=0, (n-1)\3, binomial(n-1,3*k) * a(3*k) * a(n-1-3*k)));
for(n=0, 20, print1(a(n),", "));

a_vector(n) = my(v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=0, (i-1)\3, binomial(i-1, 3*j)*v[3*j+1]*v[i-3*j])); v;
a_vector(M)

print("------------------");

\\ E.g.f. A(x) satisfies A'(x) = A(x) * (A(x) + A(w*x) + A(w^2*x))/3, where w = exp(2*Pi*i/3).
w = exp(2*Pi*I/3);
v = sum(k=0, M, a(k) * x^k/k!) + x*O(x^M);
deriv_v = deriv(v);
vv = sum(k=0, M, a(k) * (x^k + (w*x)^k + (w^2*x)^k)/(3*k!)) + x*O(x^M);
u = v * vv;
print(deriv_v)
print(u)
print(deriv_v - u)
M=20;

\\ a(0) = a(1) = a(2) = a(3) = 1; a(n) = a(n-1) + (n-3)*a(n-4).
a(n) = my(v=[1, 1, 1, 1]); if(n<4, v[n+1], a(n-1) + (n-3)*a(n-4));
for(n=0, 15, print1(a(n), ", "));

my(A=sum(k=0, M, a(k)*x^k) + x*O(x^M)); Vec(A)

\\ E.g.f. A(x) satisfies A'''(x) - A''(x) - x*A(x) = 0.
my(A=sum(k=0, M, a(k)*x^k/k!) + x*O(x^M)); Vec(deriv(deriv(deriv(A))) - deriv(deriv(A)) - x*A)
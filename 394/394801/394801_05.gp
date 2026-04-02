M=20;

c(n) = my(v=[1, 1, 1]); if(n<3, v[n+1], c(n-1) + (n-2)*c(n-3));
my(A=sum(k=0, M, c(k)*x^k) + x*O(x^M)); Vec(A)

\\ G.f.: B(x)/(1+x*B(x)), where B(x) is the g.f. of A394799.
my(A=sum(k=0, M, c(k)*x^k) + x*O(x^M)); Vec(A/(1+x*A))

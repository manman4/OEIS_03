\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, 
\\ otherwise b(n,m) = 1/4 * ( (4*m)^(n-m) * binomial(n,m) - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} (b(n,k) + Sum_{j=k..n} b(n,j) * b(j,k)) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n, m) = if(n < m, 0, if(n == m, 1, 1/4 * ((4*m)^(n-m) * binomial(n, m) - sum(l=m+1, n-1, (b(n, l) + sum(k=l, n, (b(n, k) + sum(j=k, n, b(n, j) * b(j, k))) * b(k, l))) * b(l, m)))));
a(n) = b(n, 1);
for(n=0, 9, print1(a(n), ", "))


a=[1, 2, -6, 64, -1000, 18456, -323456, 2260672, 234740736];
f(x) = sum(n=1, #a, a[n]*x^n/n!);
Vec(serlaplace( f(f(f(f(x)))) + x*O(x^#a) ))

my(N=20, x='x+O('x^N)); Vec(serlaplace( x*exp(4*x) ))

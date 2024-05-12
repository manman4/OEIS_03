\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, 
\\ otherwise b(n,m) = 1/4 * ( 4^(n-m) * Sum_{l=0..m} binomial(l,n-3*m+2*l) * binomial(m,l) - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} (b(n,k) + Sum_{j=k..n} b(n,j) * b(j,k)) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n, m) = if(n < m, 0, if(n == m, 1, 1/4 * (4^(n-m) * sum(l=0, m, binomial(l, n-3*m+2*l) * binomial(m, l)) - sum(l=m+1, n-1, (b(n, l) + sum(k=l, n, (b(n, k) + sum(j=k, n, b(n, j) * b(j, k))) * b(k, l))) * b(l, m)))));
a(n) = b(n, 1);
for(n=0, 9, print1(a(n), ", "))


a=[1, 1, 1, -15, 81, -159, -1695, 19857, -77775, -372351];
f(x) = sum(n=1, #a, a[n]*x^n);

b=[1, 1, 1, -3, 5, 1, -39, 117, 13, -1311, 3441, 9525, -78603, 16961];
g(x) = sum(n=1, #b, b[n]*x^n);
Vec(g(2*x)/2 + x*O(x^#b))
Vec(f(f(x)) + x*O(x^#a))

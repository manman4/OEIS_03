\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, 
\\ otherwise b(n,m) = 1/6 * ( 18^(n-m) * binomial(n+m-1,2*m-1) - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} (b(n,k) + Sum_{j=k..n} (b(n,j) + Sum_{i=j..n} (b(n,i) + Sum_{h=i..n} b(n,h) * b(h,i)) * b(i,j)) * b(j,k)) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n, m) = if(n<m, 0, if(n==m, 1, 1/6 * ( 18^(n-m) * binomial(n+m-1, 2*m-1) - sum(l=m+1, n-1, (b(n,l) + sum(k=l, n, (b(n,k) + sum(j=k, n, (b(n,j) + sum(i=j, n, (b(n,i) + sum(h=i, n, b(n,h) * b(h,i))) * b(i,j))) * b(j,k))) * b(k,l))) * b(l,m) ))));
a(n) = b(n, 1);
for(n=0, 8, print1(a(n), ", "))


my(N=20, x='x+O('x^N)); Vec( x/(1 - 18*x)^2 )


a=[1, 6, -18, 378, -5670, 52488, 930204, -55108026, 575622774];
f(x) = sum(n=1, #a, a[n]*x^n);

b=[1, 6, 9, 54, 0, -1944, 44469, -323676, -5990193, 179194032];
g(x) = sum(n=1, #b, b[n]*x^n);
Vec(g(2*x)/2 + x*O(x^#b))
Vec(f(f(x)) + x*O(x^#a))
c=[1, 2, 2, 2, 2, 0, 4, 6, -58, 100, 1052, -5924, -21972, 322020];
h(x) = sum(n=1, #c, c[n]*x^n);
Vec(h(9*x)/9 + x*O(x^#c))
Vec(f(f(f(x))) + x*O(x^#a))
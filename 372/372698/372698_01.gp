\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, otherwise 
\\ b(n,m) = 1/3 * ( 3*^(n-m) * |Stirling1(n,m)| - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} b(n,k) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n,m) = if(n<=m, n==m, 1/3 * ( 3^(n-m) * abs( stirling(n,m,1) ) - sum(l=m+1, n-1, (b(n,l) + sum(k=l, n, b(n,k) * b(k,l))) * b(l,m) )));
a(n) = b(n, 1);
for(n=0, 11, print1(a(n), ", "))


a=[1, 1, 3, 15, 103, 882, 8913, 103227, 1359162, 20445228];
f(x) = sum(n=1, #a, a[n]*x^n/n!);
print("A(A(A(x))): ", Vec(serlaplace( f(f(f(x))) + x*O(x^#a)) ))

my(N=20, x='x+O('x^N)); Vec(serlaplace( -log(1 - 3*x)/3 ))


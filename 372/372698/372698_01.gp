\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, otherwise 
\\ b(n,m) = 1/3 * ( (3*m)^(n-m) * binomial(n,m) - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} b(n,k) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n,m) = if(n<=m, n==m, 1/3 * ( (3*m)^(n-m) * binomial(n,m) - sum(l=m+1, n-1, (b(n,l) + sum(k=l, n, b(n,k) * b(k,l))) * b(l,m) )));
a(n) = b(n, 1);
for(n=0, 11, print1(a(n), ", "))


a=[1, 2, -3, 24, -255, 2916, -20475, -614592, 41925681, -1399671900];
f(x) = sum(n=1, #a, a[n]*x^n/n!);
print("A(A(A(x))): ", Vec(serlaplace( f(f(f(x))) + x*O(x^#a)) ))

my(N=20, x='x+O('x^N)); Vec(serlaplace( x*exp(3*x) ))


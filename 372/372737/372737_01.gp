\\ Define the sequence b(n,m) as follows. 
\\ If n<m, b(n,m) = 0, else if n=m, b(n,m) = 1, otherwise 
\\ b(n,m) = 1/3 * ( 9^(n-m) * binomial(n+m-1,2*m-1) - Sum_{l=m+1..n-1} (b(n,l) + Sum_{k=l..n} b(n,k) * b(k,l)) * b(l,m) ). 
\\ a(n) = b(n,1).

b(n,m) = if(n<=m, n==m, 1/3 * ( 9^(n-m) * binomial(n+m-1,2*m-1) - sum(l=m+1, n-1, (b(n,l) + sum(k=l, n, b(n,k) * b(k,l))) * b(l,m) )));
a(n) = b(n, 1);
for(n=0, 11, print1(a(n), ", "))


a=[1, 6, 9, 54, 0, -1944, 44469, -323676, -5990193, 179194032, 484654509, -105337511100];
f(x) = sum(n=1, #a, a[n]*x^n);
print("A(A(x)): ", Vec(f(f(x)) + x*O(x^#a)))
print("A(A(A(x))): ", Vec(f(f(f(x))) + x*O(x^#a)))

for(n=0, 15, print1(n * 9^(n-1), ", "))


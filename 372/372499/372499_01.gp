\\ If n<k, b(n,k) = 0, else if n=k, b(n,k) = 1, otherwise b(n,k) = 1/3 * (9^(n-k) * binomial(n+k-1,2*k-1) - Sum_{m=k+1..n-1} (b(n,m) + Sum_{j=m..n} b(n,j) * b(j,m)) * b(m,k)). a(n) = b(n,1).
b(n, k) = if(n<k, 0, if(n==k, 1, 1/3 * (9^(n-k) * binomial(n+k-1, 2*k-1) - sum(m=k+1, n-1, (b(n, m) + sum(j=m, n, b(n, j) * b(j, m))) * b(m, k)))));
a(n) = b(n, 1);
for(n=0, 12, print1(a(n), ", "))


a=[1, 6, 9, 54, 0, -1944, 44469, -323676, -5990193, 179194032, 484654509, -105337511100];
f(x) = sum(n=1, #a, a[n]*x^n);
print("A(A(x)): ", Vec(f(f(x)) + x*O(x^#a)))
print("A(A(A(x))): ", Vec(f(f(f(x))) + x*O(x^#a)))

for(n=0, 15, print1(n * 9^(n-1), ", "))




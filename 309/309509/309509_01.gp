\\ If n<k, b(n,k) = 0, else if n=k, b(n,k) = 1, otherwise b(n,k) = 1/2 * (2^(n-k) * binomial(n+k-1,2*k-1) - Sum_{m=k+1..n-1} b(n,m) * b(m,k)). a(n) = b(n,1).
b(n, k) = if(n<=k, n==k, 1/2 * (2^(n-k) * binomial(n+k-1,2*k-1) - sum(m=k+1, n-1, b(n,m) * b(m,k))));
a(n) = b(n, 1);
for(n=0, 15, print1(a(n),", ")) 


a=[1, 2, 2, 2, 2, 0, 4, 6, -58, 100, 1052, -5924, -21972, 322020, 332392, -21168682, 29068598]
f(x) = sum(n=1, #a, a[n]*x^n)
Vec(f(f(x)) + x*O(x^#a))

for(n=0, 15, print1(binomial(n, 1) * 2^(n-1), ", "))

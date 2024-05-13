default(parisize, 10000000000)

b(n, m) = if(n < m, 0, if(n == m, 1, 1/4 * ( 8^(n-m) * stirling(n, m, 2) - sum(l=m+1, n-1, (b(n, l) + sum(k=l, n, (b(n, k) + sum(j=k, n, b(n, j) * b(j, k))) * b(k, l))) * b(l, m)))));
a(n) = b(n, 1);
for(n=0, 10, print1(a(n), ", "))


a=[1, 2, -2, 8, 24, -2240, 59600, -640000];
\\ E.g.f.に注意
f(x) = sum(n=1, #a, a[n]*x^n/n!);
print("A(A(A(A(x)))): ", Vec(serlaplace(f(f(f(f(x)))) + x*O(x^#a))))

Vec(serlaplace( (exp(8*x) - 1)/8 ))



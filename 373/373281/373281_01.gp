b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
a(n, k=5) = sumdiv(n, d, d*b(d, k));

\\ for(n=1, 50, print1(b(n, 5), ", "))
\\ for(n=1, 1000, write("/Users/xxx/Desktop/b373281_1.txt", n, " ", a(n)))

\\ If n == 0 (mod 5), a(n) = 5^(n-1) + a(n/5) otherwise a(n) = 5^(n-1).
a(n) = 5^(n-1)+if(n%5==0, a(n/5));
for(n=1, 20, print1(a(n), ", "));
a_vector(n) = my(v=vector(n)); for(i=1, n, v[i]=5^(i-1)+if(i%5==0, v[i/5])); v;
a_vector(30)

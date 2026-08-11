b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
a(n, k=7) = sumdiv(n, d, d*b(d, k));

\\ for(n=1, 50, print1(b(n, 7), ", "))
\\ for(n=1, 1000, write("/Users/xxx/Desktop/b373283_1.txt", n, " ", a(n)))

\\ If n == 0 (mod 7), a(n) = 7^(n-1) + a(n/7) otherwise a(n) = 7^(n-1). 
a(n) = 7^(n-1)+if(n%7==0, a(n/7));
for(n=1, 20, print1(a(n), ", "));
a_vector(n) = my(v=vector(n)); for(i=1, n, v[i]=7^(i-1)+if(i%7==0, v[i/7])); v;
a_vector(30)

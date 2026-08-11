b(n, k) = sumdiv(n, d, (gcd(d, k)==1)*(moebius(d)*k^(n/d)))/(k*n);
a(n, k=3) = sumdiv(n, d, d*b(d, k));

for(n=1, 50, print1(b(n, 3), ", "));
for(n=1, 50, print1(a(n), ", "));

\\ If n == 0 (mod 3), a(n) = 3^(n-1) + a(n/3) otherwise a(n) = 3^(n-1).

a_vector(n) = my(v=vector(n)); for(i=1, n, v[i]=3^(i-1)+if(i%3==0, v[i/3])); v;
a_vector(30) 
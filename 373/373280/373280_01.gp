\\ If n == 0 (mod 4), a(n) = 4^(n-1) + a(n/4) otherwise a(n) = 4^(n-1).
a(n) = 4^(n-1)+if(n%4==0, a(n/4));
for(n=1, 20, print1(a(n), ", "));
a_vector(n) = my(v=vector(n)); for(i=1, n, v[i]=4^(i-1)+if(i%4==0, v[i/4])); v;
a_vector(30)
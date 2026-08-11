\\ If n == 0 (mod 6), a(n) = 6^(n-1) + a(n/6) otherwise a(n) = 6^(n-1).
a(n) = 6^(n-1)+if(n%6==0, a(n/6));
for(n=1, 20, print1(a(n), ", "));
a_vector(n) = my(v=vector(n)); for(i=1, n, v[i]=6^(i-1)+if(i%6==0, v[i/6])); v; 
a_vector(30)
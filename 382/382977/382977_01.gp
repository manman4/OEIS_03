\\ This sequence is obtained from the generalized Euler transform in A266964 by taking f(n) = 1, g(n) = 2^n - 1.
f(n) = 1;
g(n) = 2^n-1;
a_vector(n) = my(b=vector(n, k, sumdiv(k, d, d*f(d)*g(d)^(k/d))), v=vector(n+1)); v[1]=1; for(i=1, n, v[i+1]=sum(j=1, i, b[j]*v[i-j+1])/i); v;
a_vector(30)
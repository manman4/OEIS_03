M=22;

Pochhammer(x, n) = prod(k=0, n-1, x+k);
a(n) = my(v=2^(1/3), w=(-1+sqrt(3)*I)/2); round((Pochhammer(v, n)+w*Pochhammer(v*w, n)+w^2*Pochhammer(v*w^2, n))/(3*v^2));
for(n=0, M, print1(a(n), ", "));
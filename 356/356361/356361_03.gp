M=21;

Pochhammer(x, n) = prod(k=0, n-1, x+k);
a(n) = my(v=n^(1/3), w=(-1+sqrt(3)*I)/2); round(Pochhammer(v, n)+Pochhammer(v*w, n)+Pochhammer(v*w^2, n))/3;
for(n=0, M, print1(a(n), ", "));